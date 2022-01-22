#include "app/shell/shell.h"
#include "app/shell/handlers.h"
#include "system/dbgio.h"


static int help(int argc, char *argv[]);


const shell_command_t shell_commands[] = {
    {"help", help, "print help"},
    {"hello", hello_test, "just print hello"},
    {"rtc", rtc_test, "print time"},
    {"mqtt", mqtt_test, "run mqtt test"},
    {"ping", ping_test, "ping"},
    {"lis", accelero_test, "accelerometer test"},
};

const size_t shell_commands_num = (sizeof(shell_commands) / sizeof(shell_command_t));


static int help(int argc, char *argv[])
{
    for (size_t i = 0; i < shell_commands_num; i++) {
        const shell_command_t *curr = &shell_commands[i];
        CONSOLE_OUT("\t%s: %s\r\n", curr->cmd, curr->help);
    }

    return 0;
}


int shell_exec_cmd(int argc, char *argv[])
{
    const char *cmd = argv[0];

    for (size_t i = 0; i < shell_commands_num; i++) {
        const shell_command_t *curr = &shell_commands[i];

        if (strcmp(curr->cmd, cmd) == 0) {
            curr->handler(argc, argv);
            return 0;
        }
    }

    CONSOLE_OUT("Unknown command!\r\n");
    shell_commands[0].handler(argc, argv);  // Print help

    return 0;
}
