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
            .enable = true,
            .line = SPI1_IRQn,
            .main_prio = 6,
            .sub_prio = 0,
        },
        .rcc = {
            .clk_reg = &RCC->APB2ENR,
            .rst_reg = &RCC->APB2RSTR,
            .en_bit = RCC_APB2ENR_SPI1EN,
        },
        .cs_pin   = ACCELERO_SPI_CS,
        .miso_pin = ACCELERO_SPI_MISO,
        .mosi_pin = ACCELERO_SPI_MOSI,
        .sclk_pin = ACCELERO_SPI_SCLK,
    }
};

const size_t spi_list_len = (sizeof(spi_list) / sizeof(spi_conf_t));
