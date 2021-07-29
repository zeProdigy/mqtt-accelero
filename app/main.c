#include "usb/usbd_desc.h"
#include "usb/core/usbd_core.h"

#include "system/cpu.h"
#include "system/gpio.h"
#include "system/uart.h"
#include "system/rtc.h"
#include "system/timer.h"
#include "system/dbgio.h"
#include "system/uptimer.h"
#include "system/cmd_line.h"

#include "osal/osal.h"

#include "app/shell/shell.h"


static USBD_HandleTypeDef usb_dev;


void print_banner(void)
{
    const char *banner = "\
     _    _  __  _   _      _    ____              _\r\n\
    / \\  | |/ / | \\ | | ___| |_ / ___|__ _ _ __ __| |\r\n\
   / _ \\ | ' /  |  \\| |/ _ \\ __| |   / _` | '__/ _` |\r\n\
  / ___ \\| . \\  | |\\  |  __/ |_| |__| (_| | | | (_| |\r\n\
 /_/   \\_\\_|\\_\\ |_| \\_|\\___|\\__|\\____\\__,_|_|  \\__,_|";

    dbgio_printf("%s\r\n", banner);
}


void blinker(__attribute__((unused)) void const *params)
{
    uint32_t period = 1000;
    gpio_init(GPIO_GREEN_LED);
    while(true) {
        gpio_toggle(GPIO_GREEN_LED);
        osal_thread_sleep(period);
    }
}


void cmd_line(__attribute__((unused)) void const *params)
{
    cmd_line_process(shell_exec_cmd);
}


int main(void) {
    cpu_clock_setup();
    cpu_systick_setup();
    cpu_features_setup();

    rtc_setup();
    uart_init(DEBUG_UART, 115200);
    uptimer_setup();
    uptimer_start();

    print_banner();

    if (osal_thread_create("blinker", blinker, 128, THREAD_PRIORITY_LOW, NULL) == NULL) {
        CONSOLE_ERROR("Can't create blinker thread");
    }

    if (osal_thread_create("cmd_line", cmd_line, 512, THREAD_PRIORITY_NORMAL, NULL) == NULL) {
        CONSOLE_ERROR("Can't create cmd_line thread");
    }

    osal_kernel_start();

    while (true) {}
}
