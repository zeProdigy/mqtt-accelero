#include <sys/time.h>

#include "system/dbgio.h"
#include "system/rtc.h"


int rtc_test(int argc, char *argv[])
{
    struct timeval time;
    gettimeofday(&time, NULL);
    CONSOLE_OUT("\t%ds %03dms\r\n", (uint32_t)time.tv_sec, (uint32_t)time.tv_usec / 1000);
    return 0;
}
