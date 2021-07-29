#include <sys/time.h>

#include "system/dbgio.h"
#include "system/rtc.h"


int rtc_test(int argc, char *argv[])
{
    struct timeval time;
    gettimeofday(&time, NULL);
    CONSOLE_LOG("sec = %d", time.tv_sec);
    return 0;
}
