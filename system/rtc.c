#include "system/rtc.h"
#include "system/dbgio.h"


static RTC_HandleTypeDef rtc = {0};


static int rtc_clock_setup(void)
{
    RCC_PeriphCLKInitTypeDef periph_clk = {0};

    periph_clk.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    periph_clk.RTCClockSelection    = RCC_RTCCLKSOURCE_HSE_DIV8;
    
    if (HAL_RCCEx_PeriphCLKConfig(&periph_clk) != HAL_OK) { 
        return 1;
    }

    __HAL_RCC_RTC_ENABLE();

    return 0;
}


int rtc_setup(void)
{
    if (rtc_clock_setup()) {
        return 1;
    }

    rtc.Instance            = RTC;
    rtc.Init.HourFormat     = RTC_HOURFORMAT_24;
    // RTC тактируется от HSE, входная частота 1 MHz. Делителями настраиваем 1 Hz
    rtc.Init.AsynchPrediv   = 124;
    rtc.Init.SynchPrediv    = 7999;

    if (HAL_RTC_Init(&rtc) != HAL_OK) {
        CONSOLE_ERROR("Init error");
        return 1;
    }

    RTC_TimeTypeDef time = {0};

    if (HAL_RTC_SetTime(&rtc, &time, RTC_FORMAT_BIN) != HAL_OK) {
        CONSOLE_ERROR("Set time error");
        return 1;
    }

    HAL_RTCEx_EnableBypassShadow(&rtc);

    return 0;
}


uint32_t rtc_get_timestamp(void)
{
    RTC_TimeTypeDef time;
    HAL_RTC_GetTime(&rtc, &time, RTC_FORMAT_BIN);
    return (time.Hours * 60 * 60) + (time.Minutes * 60) + time.Seconds;
}
