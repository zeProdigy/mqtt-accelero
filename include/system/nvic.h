#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "hal/stm32f4xx.h"


typedef struct {
    bool enable;
    IRQn_Type line;
    uint8_t main_prio;
    uint8_t sub_prio;
} nvic_conf_t;
