#include <system/gpio.h>

gpio_conf_t gpio_list[] = {
    // GPIO_GREEN_LED
    {
        .base = GPIOD,
        .conf = {
            .Pin = GPIO_PIN_12,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
        },
        .clk_en = RCC_AHB1ENR_GPIODEN
    },

    // GPIO_RED_LED
    {
        .base = GPIOD,
        .conf = {
            .Pin = GPIO_PIN_14,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
        },
        .clk_en = RCC_AHB1ENR_GPIODEN
    },

    // GPIO_ORANGE_LED
    {
        .base = GPIOD,
        .conf = {
            .Pin = GPIO_PIN_13,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
        },
        .clk_en = RCC_AHB1ENR_GPIODEN
    },

    // GPIO_BLUE_LED
    {
        .base = GPIOD,
        .conf = {
            .Pin = GPIO_PIN_15,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
        },
        .clk_en = RCC_AHB1ENR_GPIODEN
    },

    // DEBUG_UART_RX_PIN
    {
        .base = GPIOA,
        .conf = {
            .Pin = GPIO_PIN_3,
            .Mode = GPIO_MODE_AF_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
            .Alternate = GPIO_AF7_USART2,
        },
        .clk_en = RCC_AHB1ENR_GPIOAEN
    },

    // DEBUG_UART_TX_PIN
    {
        .base = GPIOA,
        .conf = {
            .Pin = GPIO_PIN_2,
            .Mode = GPIO_MODE_AF_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
            .Alternate = GPIO_AF7_USART2,
        },
        .clk_en = RCC_AHB1ENR_GPIOAEN
    },

    // ACCELERO_SPI_SCLK
    {
        .base = GPIOA,
        .conf = {
            .Pin = GPIO_PIN_5,
            .Mode = GPIO_MODE_AF_PP,
            .Pull = GPIO_PULLDOWN,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
            .Alternate = GPIO_AF5_SPI1,
        },
        .clk_en = RCC_AHB1ENR_GPIOAEN
    },

    // ACCELERO_SPI_MISO
    {
        .base = GPIOA,
        .conf = {
            .Pin = GPIO_PIN_6,
            .Mode = GPIO_MODE_AF_PP,
            .Pull = GPIO_PULLDOWN,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
            .Alternate = GPIO_AF5_SPI1,
        },
        .clk_en = RCC_AHB1ENR_GPIOAEN
    },

    // ACCELERO_SPI_MOSI
    {
        .base = GPIOA,
        .conf = {
            .Pin = GPIO_PIN_7,
            .Mode = GPIO_MODE_AF_PP,
            .Pull = GPIO_PULLDOWN,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
            .Alternate = GPIO_AF5_SPI1,
        },
        .clk_en = RCC_AHB1ENR_GPIOAEN
    },

    // ACCELERO_SPI_CS
    {
        .base = GPIOE,
        .conf = {
            .Pin = GPIO_PIN_3,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
        },
        .clk_en = RCC_AHB1ENR_GPIOEEN
    },
};
