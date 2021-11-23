#include <system/gpio.h>


extern gpio_conf_t gpio_list[];


void gpio_init(GPIO_ID_T id)
{
    gpio_conf_t *gpio = (gpio_conf_t *)&gpio_list[id];
    SET_BIT(RCC->AHB1ENR, gpio->clk_en);
    HAL_GPIO_Init(gpio->base, &gpio->conf);
}


void gpio_deinit(GPIO_ID_T id)
{
    gpio_conf_t *gpio = (gpio_conf_t *)&gpio_list[id];
    HAL_GPIO_DeInit(gpio->base, gpio->conf.Pin);
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


bool gpio_get(GPIO_ID_T id)
{
    gpio_conf_t *gpio = (gpio_conf_t *)&gpio_list[id];
    return (HAL_GPIO_ReadPin(gpio->base, gpio->conf.Pin) == GPIO_PIN_SET) ? 1 : 0;
}
