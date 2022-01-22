#include "system/cpu.h"
#include "system/gpio.h"
#include "system/uart.h"
#include "system/rtc.h"
#include "system/timer.h"
#include "system/dbgio.h"
#include "system/usb_ecm.h"
#include "system/uptimer.h"
#include "system/cmd_line.h"
#include "system/net.h"
#include "system/accelero.h"

#include "osal/osal.h"

#include "app/shell/shell.h"
#include "app/server.h"


void print_banner(void)
{
    const char *banner = "\
\e[0;32m           ____                _ _\r\n\
\e[0;32m   _______|  _ \\ _ __ ___   __| (_) __ _ _   _\r\n\
\e[0;33m  |_  / _ \\ |_) | '__/ _ \\ / _` | |/ _` | | | |\r\n\
\e[0;34m   / /  __/  __/| | | (_) | (_| | | (_| | |_| |\r\n\
\e[0;35m  /___\\___|_|   |_|  \\___/ \\__,_|_|\\__, |\\__, |\r\n\
\e[0;35m                                   |___/ |___/\e[0m\r\n";

    CONSOLE_OUT("%s", banner);
    CONSOLE_OUT("  %s\r\n\n", PROJECT_VER);
}


void blinker(__attribute__((unused)) void const *params)
{
    uint32_t period = 1000;
    gpio_init(GPIO_BLUE_LED);
    while(true) {
        gpio_toggle(GPIO_BLUE_LED);
        osal_thread_sleep(period);
    }
}


void cmd_line(__attribute__((unused)) void const *params)
{
    cmd_line_process(shell_exec_cmd);
}


void app(const void *args)
{
    net_init();

    if (accelero_init()) {
        CONSOLE_ERROR("Can't init accelero");
    }

    if (usb_ecm_init()) {
        CONSOLE_ERROR("Can't init usb ecm");
    }

    if (osal_thread_create("blinker", blinker, 128, THREAD_PRIORITY_LOW, NULL) == NULL) {
        CONSOLE_ERROR("Can't create blinker thread");
    }

    if (osal_thread_create("cmd_line", cmd_line, 512, THREAD_PRIORITY_NORMAL, NULL) == NULL) {
        CONSOLE_ERROR("Can't create cmd_line thread");
    }

    if (osal_thread_create("srv_conn", mqtt_server_connection, 512, THREAD_PRIORITY_NORMAL, NULL) == NULL) {
        CONSOLE_ERROR("Can't create srv_conn thread");
    }

    osal_thread_delete(osal_thread_self());
}


int main(void) {
    cpu_clock_setup();
    cpu_systick_setup();
    cpu_features_setup();

    dbgio_init();

    if (rtc_setup()) {
        CONSOLE_LOG("Can't init RTC");
    }

    uptimer_setup();
    uptimer_start();

    print_banner();

    if (osal_thread_create("app", app, 256, THREAD_PRIORITY_NORMAL, NULL) == NULL) {
        CONSOLE_ERROR("Can't create app thread");
    }

    osal_kernel_start();

    while (true) {}
}
