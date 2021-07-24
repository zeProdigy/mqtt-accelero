#include "system/uart.h"


extern uart_conf_t uart_list[];


int uart_init(UART_ID_T id, size_t baud_rate)
{
    uart_conf_t *uart = (uart_conf_t *)&uart_list[id];

    gpio_init(uart->rx_pin_id);
    gpio_init(uart->tx_pin_id);

    SET_BIT(RCC->APB1ENR, uart->clk);

    if (HAL_UART_Init(&uart->handler)) {
        return 1;
    }

    if (uart->irq.enable) {
        uart->rx_sem = osal_semaphore_create();
        if (uart->rx_sem == NULL) {
            return 1;
        }

        HAL_NVIC_SetPriority(uart->irq.line, uart->irq.main_prio, uart->irq.sub_prio);
        HAL_NVIC_EnableIRQ(uart->irq.line);
    }

    return 0;
}


int uart_write_polling(UART_ID_T id, const void *data, size_t size)
{
    uart_conf_t *uart = (uart_conf_t *)&uart_list[id];
    return HAL_UART_Transmit(&uart->handler, (uint8_t *)data, size, -1);
}


int uart_read_polling(UART_ID_T id, void *buf, size_t size)
{
    uart_conf_t *uart = (uart_conf_t *)&uart_list[id];
    return HAL_UART_Receive(&uart->handler, (uint8_t *)buf, size, -1);
}


int uart_read_interrupt(UART_ID_T id, void *buf, size_t size)
{
    uart_conf_t *uart = (uart_conf_t *)&uart_list[id];

    __HAL_UART_ENABLE_IT(&uart->handler, UART_IT_RXNE);

    if (HAL_UART_Receive_IT(&uart->handler, (uint8_t *)buf, size)) {
        return 1;
    }

    osal_semaphore_wait(uart->rx_sem, OSAL_MAX_TIMEOUT);

    __HAL_UART_DISABLE_IT(&uart->handler, UART_IT_RXNE);

    return 0;
}


int uart_deinit(UART_ID_T id)
{
    uart_conf_t *uart = (uart_conf_t *)&uart_list[id];
    return HAL_UART_DeInit(&uart->handler);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    for (size_t n = 0; n < uart_list_len; n++) {
        uart_conf_t *uart = (uart_conf_t *)&uart_list[n];
        if (huart == &uart->handler) {
            osal_semaphore_release(uart->rx_sem);
        }
    }
}

// унести в отдельный файл
void USART2_IRQHandler(void)
{
    uart_conf_t *uart = (uart_conf_t *)&uart_list[0];
    HAL_UART_IRQHandler(&uart->handler);
}
