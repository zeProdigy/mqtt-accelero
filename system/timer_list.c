#include <system/timer.h>

timer_conf_t timer_list[] = {
    // CLOCK_TIMER
    {
        .mode = TIMER_MODE_COUNTER,
        .handler = {
            .Instance = TIM5,
            .Init = {
                .Period = 1000000 - 1,  // Частота прерываний 1 Гц
                .Prescaler = 84 - 1,    // Частота таймера 1 МГц
                .CounterMode = TIM_COUNTERMODE_UP,
                .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE,
            }
        },
        .irq = {
            .enable = true,
            .line = TIM5_IRQn,
            .main_prio = 6,
            .sub_prio = 0,
        },
        .clk_en = RCC_APB1ENR_TIM5EN,
    },
};

const size_t timer_list_len = (sizeof(timer_list) / sizeof(timer_conf_t));
