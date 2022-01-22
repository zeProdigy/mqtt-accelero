#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


typedef struct shell_command shell_command_t;

struct shell_command {
    const char *cmd;
    int (*handler)(int argc, char *argv[]);
    const char *help;
};


int shell_exec_cmd(int argc, char *argv[]);
