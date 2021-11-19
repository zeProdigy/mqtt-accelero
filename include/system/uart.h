#pragma once

#include <stdint.h>

#include "system/gpio.h"
#include "system/nvic.h"
#include "system/uart_list.h"
#include "hal/stm32f4xx.h"
#include "osal/osal.h"


typedef struct {
    UART_HandleTypeDef handler;
    nvic_conf_t irq;
    uint32_t clk;
    GPIO_ID_T tx_pin_id;
    GPIO_ID_T rx_pin_id;
    osal_semaphore_t rx_sem;
} uart_conf_t;


int uart_init(UART_ID_T id, size_t baud_rate);
int uart_write_polling(UART_ID_T id, const void *data, size_t size);
int uart_read_polling(UART_ID_T id, void *buf, size_t size);
int uart_read_interrupt(UART_ID_T id, void *buf, size_t size);
int uart_deinit(UART_ID_T id);
