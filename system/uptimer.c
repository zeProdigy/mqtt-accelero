#include "system/uptimer.h"
#include "system/timer.h"


static uint32_t sec;


int uptimer_setup(void)
{
    if (timer_init(CLOCK_TIMER)) {
        return 1;
    }

    sec = 0;

    return 0;
}


int uptimer_start(void)
{
    return timer_start(CLOCK_TIMER);
}


void uptimer_update(void)
{
    sec++;
}


uint32_t uptimer_get_sec(void)
{
    return sec;
}


uint32_t uptimer_get_msec(void)
{
    return timer_get_counter(CLOCK_TIMER);
}
