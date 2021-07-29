#include "system/dbgio.h"
#include "microrl.h"


static void print_cb(const char *str)
{
    dbgio_printf("%s", str);
}


static void sigint_handler(void)
{
    dbgio_printf("Catch ^C!\r\n");
}


void cmd_line_process(int (*shell_exec)(int, const char *const *))
{
    microrl_t rl;

    microrl_init(&rl, print_cb);
    microrl_set_execute_callback(&rl, shell_exec);
    microrl_set_sigint_callback(&rl, sigint_handler);

    while (true) {
        char ch = dbgio_getchar();
        microrl_insert_char(&rl, ch);
    }
}
