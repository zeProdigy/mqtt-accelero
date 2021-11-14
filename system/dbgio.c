#include <stdarg.h>
#include <sys/time.h>

#include "printf.h"
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

    if (timestamp) {
        struct timeval time;
        gettimeofday(&time, NULL);
        printf("%05d:%03d ", (uint32_t)time.tv_sec, (uint32_t)time.tv_usec / 1000);
    }

    if (func && line > 0) {
        printf("[%s:%d] ", func, line);
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


void dbgio_print_memory(void *data, size_t size, const char *format, ...)
{
    size_t lines = size / 16 + ((size % 16 == 0) ? 0 : 1);
    size_t countdown = size;

    if (format) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\r\n");
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


// freertos assertion hook
void vApplicationAssertionHook(const char* file, uint32_t line)
{   
    __disable_irq();
    CONSOLE_ASSERT("Freertos assert: %s:%d", file, line);
    while(true);
}


// st-hal assertion hook
void assert_failed(uint8_t* file, uint32_t line)
{
    __disable_irq();
    CONSOLE_ASSERT("ST-HAL assert: %s:%d", file, line);
    while(true);
}


// newlib assertion hook
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
    __disable_irq();

    printf("%s", CLR_YELLOW);
    printf("\r\nAssertion failed!\r\n");
    printf("\texpr: %s\r\n", failedexpr);
    printf("\tfile: %s:%d\r\n", file, line);
    printf("\tfunc: %s\r\n", func);
    printf("%s", CLR_DEFAULT);

    while(true);
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
