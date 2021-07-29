#include <sys/time.h>
#include "system/uptimer.h"


int _gettimeofday(struct timeval *tv, void *tz)
{
    tv->tv_sec = (time_t)uptimer_get_sec();
    tv->tv_usec = (suseconds_t)uptimer_get_msec();
    return 0;
}
