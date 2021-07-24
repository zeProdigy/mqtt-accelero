#include "stm32f4xx.h"


int cpu_clock_setup(void)
{
    RCC_OscInitTypeDef osc;
    RCC_ClkInitTypeDef clk;

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    osc.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState            = RCC_HSI_ON;
    osc.HSICalibrationValue = 16;
    osc.PLL.PLLState        = RCC_PLL_ON;
    osc.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    osc.PLL.PLLM            = 8;
    osc.PLL.PLLN            = 168;
    osc.PLL.PLLP            = RCC_PLLP_DIV2;
    osc.PLL.PLLQ            = 7;

    if (HAL_RCC_OscConfig(&osc) != HAL_OK) {
        return 1;
    }

    clk.ClockType       = RCC_CLOCKTYPE_HCLK |
                          RCC_CLOCKTYPE_SYSCLK |
                          RCC_CLOCKTYPE_PCLK1 |
                          RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider   = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider  = RCC_HCLK_DIV4;
    clk.APB2CLKDivider  = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_5) != HAL_OK) {
        return 1;
    }

    return 0;
}


void cpu_systick_setup(void)
{
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


void cpu_features_setup(void)
{
    /* Enable branch prediction */
    SCB->CCR |= (1 << 18);
}
