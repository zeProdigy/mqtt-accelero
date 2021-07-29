#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "system/nvic.h"
#include "system/timer_list.h"
#include "hal/stm32f4xx.h"


typedef enum {
    TIMER_MODE_COUNTER,
} TIMER_MODE_T;

typedef struct {
    TIMER_MODE_T mode;
    TIM_HandleTypeDef handler;
    nvic_conf_t irq;
    uint32_t clk_en;
} timer_conf_t;


extern timer_conf_t timer_list[];


int timer_init(TIMER_ID_T id);
int timer_start(TIMER_ID_T id);
uint32_t timer_get_counter(TIMER_ID_T id);
