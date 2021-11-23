#include "hal/stm32f4xx.h"
#include "system/timer.h"
#include "system/uptimer.h"
#include "system/usb_ecm.h"
#include "system/spi.h"
#include "osal/osal.h"


void SysTick_Handler(void)
{
    HAL_IncTick();
    osal_kernel_systick_handler();
}


void TIM5_IRQHandler(void)
{
    timer_conf_t *timer = (timer_conf_t *)&timer_list[CLOCK_TIMER];
    HAL_TIM_IRQHandler(&timer->handler);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htimer)
{
    for (size_t n = 0; n < timer_list_len; n++) {
        if (htimer->Instance == TIM5) {
            uptimer_update();
        }
    }
}


void OTG_FS_IRQHandler(void)
{
    usb_ecm_irq_handler();
}


void SPI1_IRQHandler(void)
{
    spi_irq_handler(ACCELERO_SPI);
}
