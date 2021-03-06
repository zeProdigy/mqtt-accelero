#pragma once

typedef enum {
    GPIO_GREEN_LED,
    GPIO_RED_LED,
    GPIO_ORANGE_LED,
    GPIO_BLUE_LED,

    DEBUG_UART_RX_PIN,
    DEBUG_UART_TX_PIN,

    ACCELERO_SPI_SCLK,
    ACCELERO_SPI_MISO,
    ACCELERO_SPI_MOSI,
    ACCELERO_SPI_CS,
} GPIO_ID_T;
