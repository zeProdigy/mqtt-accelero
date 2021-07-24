#include <stdarg.h>

#include "3rdparty/tiny-printf/printf.h"
#include "system/dbgio.h"
#include "system/uart.h"
#include "osal/osal.h"


void dbgio_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}


void dbgio_printf_extended(bool use_mutex,
                           bool timestamp,
                           bool new_line,
                           const char *func,
                           int line,
                           const char *color,
                           const char *format, ...)
{
    va_list args;

    if (func && line > 0) {
        printf("%s:%d ", func, line);
    }

    if (color) {
        printf("%s", color);
    }

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    if (color) {
        printf("%s", CLR_DEFAULT);
    }

    if (new_line) {
        printf("\r\n");
    }
}


void dbgio_print_memory(void *data, size_t size, const char *desc)
{
    size_t lines = size / 16 + ((size % 16 == 0) ? 0 : 1);
    size_t countdown = size;

    if (desc) {
        printf("%s\r\n", desc);
    }

    for (size_t line = 0; line < lines; line++) {
        uint8_t *pdata = &((uint8_t *)data)[16 * line];
        size_t len = (countdown > 16) ? 16 : countdown;

        printf("%04lx  ", (uint32_t)pdata);

        for (size_t n = 0; n < 16; n++) {
            if (n == 8) {
                printf(" ");
            }

            if (n < len) {
                printf("%02x ", pdata[n]);
            } else {
                printf("   ");
            }
        }

        printf(" ");

        for (size_t n = 0; n < 16; n++) {
            char byte = (n < len) ? (char)pdata[n] : '.';
            char ch = (byte >= ' ' && byte <= '~') ? byte : '.';
            printf("%c", ch);
        }

        printf("\r\n");

        countdown -= 16;
    }
}


char dbgio_getchar(void)
{
    char ch;
    uart_read_interrupt(DEBUG_UART, &ch, sizeof(ch));
    return ch;
}


// Реализация хука для FreeRTOS
void vApplicationStackOverflowHook(__attribute__((unused)) TaskHandle_t xTask,
                                   char *pcTaskName)
{
    // osal_thread_stop_all();
    dbgio_printf_extended(false, true, true, NULL, 0, "Thread \"%s\" stack overflow!", pcTaskName);
    while (true) {}
}


// Для работы 3rdparty библиотеки printf
void _putchar(char character)
{
    const char r = '\r';

    if (character == '\n') {
        uart_write_polling(DEBUG_UART, &r, sizeof(r));
    }

    uart_write_polling(DEBUG_UART, &character, sizeof(character));
}
