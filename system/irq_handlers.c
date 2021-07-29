#include "hal/stm32f4xx.h"
#include "system/timer.h"
#include "system/uptimer.h"


// void TIM5_IRQHandler(void);


// void TIM5_IRQHandler(void)
// {
//     timer_conf_t *timer = (timer_conf_t *)&timer_list[CLOCK_TIMER];
//     HAL_TIM_IRQHandler(&timer->handler);
// }


// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htimer)
// {
//     for (size_t n = 0; n < timer_list_len; n++) {
//         if (htimer->Instance == TIM5) {
//             uptimer_update();
//         }
//     }
// }
