#pragma once

#include <stdint.h>

#include "system/nvic.h"
#include "system/rcc.h"
#include "system/gpio.h"
#include "system/spi_list.h"
#include "hal/stm32f4xx.h"
#include "osal/osal.h"


typedef struct {
    SPI_HandleTypeDef handler;

    nvic_conf_t irq;
    rcc_conf_t rcc;

    osal_semaphore_t rx_sem;
    osal_semaphore_t tx_sem;
    osal_mutex_t mut;

    GPIO_ID_T cs_pin;
    GPIO_ID_T miso_pin;
    GPIO_ID_T mosi_pin;
    GPIO_ID_T sclk_pin;
} spi_conf_t;


int spi_init(SPI_ID_T id);
int spi_deinit(SPI_ID_T id);
int spi_write_interrupt(SPI_ID_T id, const void *data, size_t size);
int spi_read_interrupt(SPI_ID_T id, void *buf, size_t size);
int spi_exchange_interrupt(SPI_ID_T id,
                           const void *out_data, void *in_buf, size_t size);
int spi_exchange_polling(SPI_ID_T id,
                         const void *out_data, size_t out_size,
                         void *in_buf, size_t in_size);
void spi_cs_enable(SPI_ID_T spi);
void spi_cs_disable(SPI_ID_T spi);
void spi_irq_handler(SPI_ID_T id);