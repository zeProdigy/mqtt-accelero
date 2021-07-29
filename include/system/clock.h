#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "hal/stm32f4xx.h"


typedef struct {
    uint32_t reg;
    uint32_t en_bit;
} clk_conf_t;
