#include "system/dbgio.h"
#include "microrl.h"

static void print_cb(const char *str)
{
    dbgio_printf("%s", str);
}


static int cmd_handler(int argc, const char *const *argv)
{
    dbgio_printf("argc = %d, argv = %s\r\n", argc, argv[0]);
    return 0;
}


static void sigint_handler(void)
{
    dbgio_printf("Catch ^C!\r\n");
}

void cmd_line_process(void)
{
    microrl_t rl;

    microrl_init(&rl, print_cb);
    microrl_set_execute_callback(&rl, cmd_handler);
    microrl_set_sigint_callback(&rl, sigint_handler);

    while (true) {
        char ch = dbgio_getchar();
        microrl_insert_char(&rl, ch);
    }
}
