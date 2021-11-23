#include <system/spi.h>

spi_conf_t spi_list[] = {
    // ACCELERO_SPI
    {
        .handler = {
            .Instance = SPI1,
            .Init = {
                .Mode = SPI_MODE_MASTER,
                .CLKPolarity = SPI_POLARITY_LOW,
                .CLKPhase = SPI_PHASE_1EDGE,
                .NSS = SPI_NSS_SOFT,
                .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
            }
        },
        .irq = {

        },
        .rcc = {

        },
        .cs_pin = ,
        .miso_pin = ,
        .mosi_pin = ,
        .sclk_pin = ,
    }
};

const size_t spi_list_len = (sizeof(spi_list) / sizeof(spi_conf_t));
