#pragma once

#include <stdint.h>

#include "system/gpio_list.h"
#include "hal/stm32f4xx.h"


typedef struct {
    GPIO_TypeDef *base;
    GPIO_InitTypeDef conf;
    uint32_t clk_en;
} gpio_conf_t;


void gpio_init(GPIO_ID_T id);
void gpio_set(GPIO_ID_T id);
void gpio_reset(GPIO_ID_T id);
void gpio_toggle(GPIO_ID_T id);
