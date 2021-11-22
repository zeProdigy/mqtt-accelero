#include "system/spi.h"
#include "osal/osal.h"


extern spi_conf_t spi_list[];


int spi_init(SPI_ID_T id)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];

    gpio_init(spi->cs_pin);
    gpio_init(spi->miso_pin);
    gpio_init(spi->mosi_pin);
    gpio_init(spi->sclk_pin);

    // SET_BIT(RCC->APB1ENR, spi->clk); // TODO

    if (HAL_SPI_Init(&spi->handler)) {
        return 1;
    }

    spi->rx_sem = osal_semaphore_create();
    if (spi->rx_sem == NULL) {
        return 1;
    }

    spi->tx_sem = osal_semaphore_create();
    if (spi->tx_sem == NULL) {
        return 1;
    }

    spi->mut = osal_mutex_create();
    if (spi->mut == NULL) {
        return 1;
    }

    HAL_NVIC_SetPriority(spi->irq.line, spi->irq.main_prio, spi->irq.sub_prio);
    HAL_NVIC_EnableIRQ(spi->irq.line);

    return 0;
}


int spi_deinit(SPI_ID_T id)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];
    return 0;
}


int spi_write_interrupt(SPI_ID_T id, const void *data, size_t size)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];
    return 0;
}


int spi_read_interrupt(SPI_ID_T id, void *buf, size_t size)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];
    return 0;
}


int spi_exchange_interrupt(SPI_ID_T id,
                           const void *out_data, size_t out_size,
                           void *in_buf, size_t in_size)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];
    return 0;
}
