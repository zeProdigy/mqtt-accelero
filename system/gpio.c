#include <system/gpio.h>


extern gpio_conf_t gpio_list[];


void gpio_init(GPIO_ID_T id)
{
    gpio_conf_t *gpio = (gpio_conf_t *)&gpio_list[id];
    SET_BIT(RCC->AHB1ENR, gpio->clk_en);
    HAL_GPIO_Init(gpio->base, &gpio->conf);
    
}


void gpio_set(GPIO_ID_T id)
{
    gpio_conf_t *gpio = (gpio_conf_t *)&gpio_list[id];
    HAL_GPIO_WritePin(gpio->base, gpio->conf.Pin, GPIO_PIN_SET);
}


void gpio_reset(GPIO_ID_T id)
{
    gpio_conf_t *gpio = (gpio_conf_t *)&gpio_list[id];
    HAL_GPIO_WritePin(gpio->base, gpio->conf.Pin, GPIO_PIN_RESET);
}


void gpio_toggle(GPIO_ID_T id)
{
    gpio_conf_t *gpio = (gpio_conf_t *)&gpio_list[id];
    HAL_GPIO_TogglePin(gpio->base, gpio->conf.Pin);
}
