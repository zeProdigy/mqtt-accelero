#include "app/shell/shell.h"
#include "app/shell/hello_test.h"
#include "app/shell/rtc_test.h"
#include "app/shell/mqtt_test.h"


const shell_command_t shell_commands[] = {
    {"hello", hello_test, "just print hello"},
    {"rtc", rtc_test, "print time"},
    {"mqtt", mqtt_test, "run mqtt test"},
};

const size_t shell_commands_num = (sizeof(shell_commands) / sizeof(shell_command_t));


int shell_exec_cmd(int argc, const char * const *argv)
{
    const char *cmd = argv[0];

    for (size_t i = 0; i < shell_commands_num; i++) {
        const shell_command_t *curr = &shell_commands[i];

        if (strcmp(curr->cmd, cmd) == 0) {
            curr->handler(argc, (char **)argv);
        }
    }

    return 0;
}
