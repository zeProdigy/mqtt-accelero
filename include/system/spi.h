#pragma once

#include <stdint.h>

#include "system/nvic.h"
#include "system/gpio.h"
#include "hal/stm32f4xx.h"
#include "osal/osal.h"


typedef struct {
    SPI_HandleTypeDef handler;
    nvic_conf_t irq;
    uint32_t clk;
} spi_conf_t;


int spi_init(SPI_ID_T id);
int spi_write_interrupt(SPI_ID_T id, const void *data, size_t size);
int spi_read_interrupt(SPI_ID_T id, void *buf, size_t size);
