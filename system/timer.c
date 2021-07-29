#include "system/timer.h"
#include "system/dbgio.h"


int timer_init(TIMER_ID_T id)
{
    timer_conf_t *timer = (timer_conf_t *)&timer_list[id];

    SET_BIT(RCC->APB1ENR, timer->clk_en);

    switch(timer->mode) {
        case TIMER_MODE_COUNTER:
            if (HAL_TIM_Base_Init(&timer->handler) != HAL_OK) {
                return 1;
            }
            break;

        default:
            CONSOLE_LOG("Mode is not supported!");
            return 1;
    }

    if (timer->irq.enable) {
        HAL_NVIC_SetPriority(timer->irq.line, timer->irq.main_prio, timer->irq.sub_prio);
        HAL_NVIC_EnableIRQ(timer->irq.line);
    }

    return 0;
}


int timer_start(TIMER_ID_T id)
{
    timer_conf_t *timer = (timer_conf_t *)&timer_list[id];

    switch(timer->mode) {
        case TIMER_MODE_COUNTER: {
            HAL_StatusTypeDef (*start)(TIM_HandleTypeDef *) = (timer->irq.enable) ?
                HAL_TIM_Base_Start_IT : HAL_TIM_Base_Start;

            if (start(&timer->handler) != HAL_OK) {
                return 1;
            }

            break;
        }

        default:
            CONSOLE_LOG("Mode is not supported!");
            return 1;
    }

    return 0;
}


uint32_t timer_get_counter(TIMER_ID_T id)
{
    timer_conf_t *timer = (timer_conf_t *)&timer_list[id];
    return __HAL_TIM_GET_COUNTER(&timer->handler);
}
