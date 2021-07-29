#pragma once

#include <stdint.h>

#include "hal/stm32f4xx.h"
#include "hal/st-hal/stm32f4xx_ll_rtc.h"

int rtc_setup(void);
uint32_t rtc_get_timestamp(void);
