#include <system/uart.h>

uart_conf_t uart_list[] = {
    // DEBUG_UART
    {
        .handler = {
            .Instance = USART2,
            .Init = {
                .BaudRate     = 115200,
                .WordLength   = UART_WORDLENGTH_8B,
                .StopBits     = UART_STOPBITS_1,
                .Parity       = UART_PARITY_NONE,
                .HwFlowCtl    = UART_HWCONTROL_NONE,
                .Mode         = UART_MODE_TX_RX,
                .OverSampling = UART_OVERSAMPLING_16,
            },
        },
        .irq = {
            .enable = true,
            .line = USART2_IRQn,
            .main_prio = 6,
            .sub_prio = 0,
        },
        .clk = RCC_APB1ENR_USART2EN,
        .rx_pin_id = DEBUG_UART_RX_PIN,
        .tx_pin_id = DEBUG_UART_TX_PIN,
    },
};

const size_t uart_list_len = (sizeof(uart_list) / sizeof(uart_conf_t));
