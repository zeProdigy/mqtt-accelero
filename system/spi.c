#include <assert.h>

#include "system/spi.h"
#include "system/dbgio.h"
#include "osal/osal.h"


extern spi_conf_t spi_list[];


int spi_init(SPI_ID_T id)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];

    gpio_init(spi->cs_pin);
    gpio_init(spi->miso_pin);
    gpio_init(spi->mosi_pin);
    gpio_init(spi->sclk_pin);

    spi_cs_disable(id);

    CLEAR_BIT(*spi->rcc.rst_reg, spi->rcc.en_bit);
    SET_BIT(*spi->rcc.clk_reg, spi->rcc.en_bit);

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

    //HAL_NVIC_SetPriority(spi->irq.line, spi->irq.main_prio, spi->irq.sub_prio);
    //HAL_NVIC_EnableIRQ(spi->irq.line);

    return 0;
}


int spi_deinit(SPI_ID_T id)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];

    HAL_NVIC_DisableIRQ(spi->irq.line);

    HAL_SPI_DeInit(&spi->handler);

    CLEAR_BIT(*spi->rcc.clk_reg, spi->rcc.en_bit);
    SET_BIT(*spi->rcc.rst_reg, spi->rcc.en_bit);

    gpio_deinit(spi->cs_pin);
    gpio_deinit(spi->miso_pin);
    gpio_deinit(spi->mosi_pin);
    gpio_deinit(spi->sclk_pin);

    osal_semaphore_delete(spi->tx_sem);
    osal_semaphore_delete(spi->rx_sem);
    osal_mutex_delete(spi->mut);

    return 0;
}


int spi_write_interrupt(SPI_ID_T id, const void *data, size_t size)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];
    uint32_t timeout = 500;
    int ret;

    ret = HAL_SPI_Transmit_IT(&spi->handler, (uint8_t *)data, size);
    if (ret) {
        CONSOLE_ERROR("Can't start SPI IT transmit routine");
        return 1;
    }

    ret = osal_semaphore_wait(spi->tx_sem, timeout);
    if (ret) {
        CONSOLE_ERROR("Timeout reached");
        return 1;
    }

    return 0;
}


int spi_read_interrupt(SPI_ID_T id, void *buf, size_t size)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];
    uint32_t timeout = 500;
    int ret;

    ret = HAL_SPI_Receive_IT(&spi->handler, (uint8_t *)buf, size);
    if (ret) {
        CONSOLE_ERROR("Can't start SPI IT receive routine");
        return 1;
    }

    ret = osal_semaphore_wait(spi->rx_sem, timeout);
    if (ret) {
        CONSOLE_ERROR("Timeout reached");
        return 1;
    }

    return 0;
}


int spi_exchange_interrupt(SPI_ID_T id,
                           const void *out_data, void *in_buf, size_t size)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];
    uint32_t timeout = 500;
    int ret;

    ret = HAL_SPI_TransmitReceive_IT(&spi->handler,
                                     (uint8_t *)out_data, (uint8_t *)in_buf, size);
    if (ret) {
        CONSOLE_LOG("Can't start SPI transmit/receive routine");
        return 1;
    }

    ret = osal_semaphore_wait(spi->tx_sem, timeout);
    if (ret) {
        CONSOLE_ERROR("Timeout tx reached");
        return 1;
    }

    ret = osal_semaphore_wait(spi->rx_sem, timeout);
    if (ret) {
        CONSOLE_ERROR("Timeout rx reached");
        return 1;
    }

    return 0;
}


int spi_exchange_polling(SPI_ID_T id,
                         const void *out_data, size_t out_size,
                         void *in_buf, size_t in_size)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];
    uint32_t timeout = 500;
    int ret;

    if (out_size) {
        assert(out_data);

        ret = HAL_SPI_Transmit(&spi->handler, (uint8_t *)out_data, out_size, timeout);
        if (ret) {
            CONSOLE_LOG("SPI transmit timeout reached");
            return 1;
        }
    }

    if (in_size) {
        assert(in_size);

        ret = HAL_SPI_Receive(&spi->handler, (uint8_t *)in_buf, in_size, timeout);
        if (ret) {
            CONSOLE_LOG("SPI receive timeout reached");
            return 1;
        }
    }

    return 0;
}


static inline void spi_cs_control(SPI_ID_T id, bool enable)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];

    if (enable) {
        gpio_reset(spi->cs_pin);
    } else {
        gpio_set(spi->cs_pin);
    }
}


void spi_cs_enable(SPI_ID_T id)
{
    spi_cs_control(id, true);
}


void spi_cs_disable(SPI_ID_T id)
{
    spi_cs_control(id, false);
}


void spi_irq_handler(SPI_ID_T id)
{
    spi_conf_t *spi = (spi_conf_t *)&spi_list[id];

    HAL_SPI_IRQHandler(&spi->handler);
}


static spi_conf_t *get_spi_by_handle(SPI_HandleTypeDef *hspi)
{
    for (size_t n = 0; n < spi_list_len; n++) {
        spi_conf_t *spi = (spi_conf_t *)&spi_list[n];
        if (hspi == &spi->handler) {
            return spi;
        }
    }

    return NULL;
}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    spi_conf_t *spi = get_spi_by_handle(hspi);
    if (spi != NULL) {
        osal_semaphore_release(spi->tx_sem);
    }
}


void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    spi_conf_t *spi = get_spi_by_handle(hspi);
    if (spi != NULL) {
        osal_semaphore_release(spi->rx_sem);
    }
}


void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    spi_conf_t *spi = get_spi_by_handle(hspi);
    if (spi != NULL) {
        osal_semaphore_release(spi->tx_sem);
        osal_semaphore_release(spi->rx_sem);
    }
}
