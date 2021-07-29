#pragma once

#include <stdint.h>

int uptimer_setup(void);
int uptimer_start(void);
void uptimer_update(void);
uint32_t uptimer_get_sec(void);
uint32_t uptimer_get_msec(void);
