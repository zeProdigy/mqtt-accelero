#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define CLR_DEFAULT "\e[0m"

#define CLR_RED     "\e[0;31m"
#define CLR_GREEN   "\e[0;32m"
#define CLR_YELLOW  "\e[0;33m"
#define CLR_BLUE    "\e[0;34m"
#define CLR_MAGENTA "\e[0;35m"
#define CLR_CYAN    "\e[0;36m"
#define CLR_GRAY    "\e[0;37m"

#define CLR_BOLD_RED     "\e[1;31m"
#define CLR_BOLD_GREEN   "\e[1;32m"
#define CLR_BOLD_YELLOW  "\e[1;33m"
#define CLR_BOLD_BLUE    "\e[1;34m"
#define CLR_BOLD_MAGENTA "\e[1;35m"
#define CLR_BOLD_CYAN    "\e[1;36m"
#define CLR_BOLD_GRAY    "\e[1;37m"

void dbgio_printf(const char *format, ...);

void dbgio_printf_extended(bool use_mutex,
                           bool timestamp,
                           bool new_line,
                           const char *func,
                           int line,
                           const char *color,
                           const char *format, ...);

void dbgio_print_memory(void *data, size_t size, const char *desc);

char dbgio_getchar(void);


#define CONSOLE_LOG(format, args...)\
    dbgio_printf_extended(false, true, true, __FUNCTION__, __LINE__, CLR_CYAN, format, ## args)

#define CONSOLE_ERROR(format, args...)\
    dbgio_printf_extended(false, true, true, __FUNCTION__, __LINE__, CLR_RED, format, ## args)

#define CONSOLE_MEMDUMP(data, size, format, args...)\
    dbgio_print_memory(data, size, format, ## args)
