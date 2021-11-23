#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "hal/stm32f4xx.h"


typedef struct {
    volatile uint32_t *clk_reg;
    volatile uint32_t *rst_reg;
    uint32_t en_bit;
} rcc_conf_t;
