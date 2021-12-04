#include "dbgout.h"
#include <stdarg.h>
#include "uart.h"
#include "gpio.h"
#include "board_config.h"


static uint8_t tx_buffer[BOARD_DBGOUT_BUFFER_SIZE];


static void uart_msp_init_cb(void)
{
    /// Init TX pin.
    gpio_cfg_af_t cfg = {
        .assign_io_mux = BOARD_DBGOUT_UART_TX_AF,
        .pull = GPIO_PULLUP,
        .type = GPIO_AF_PP,
    };
    gpio_pin_cfg_af(BOARD_DBGOUT_UART_PIN_TX, &cfg);

    /// Init RX pin.
    cfg.assign_io_mux = BOARD_DBGOUT_UART_RX_AF;
    gpio_pin_cfg_af(BOARD_DBGOUT_UART_PIN_RX, &cfg);
}



ret_code_t dbg_init(void)
{
    ret_code_t err_code;
    err_code = uart_register_cb(BOARD_DBGOUT_UART, UART_MSP_INIT_CB_ID, uart_msp_init_cb);
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }

    uart_config_t uart_config = {
        .baud_rate = BOARD_DBGOUT_UART_SPEED,
        .word_lenght = UART_WORDLENGTH_8B,
        .stop_bits = UART_STOPBITS_1,
        .parity = UART_PARITY_NONE,
        .mode = UART_MODE_TX_RX,
        .hw_flow_ctrl = UART_HWCONTROL_NONE,
        .oversampling = UART_OVERSAMPLING_16,
        .one_bit_sampling = UART_ONE_BIT_SAMPLE_DISABLE,
        .clock_prescaller = UART_PRESCALER_DIV1,
        .prio = BOARD_DBGOUT_UART_INT_PRIO,
    };
    err_code = uart_init(BOARD_DBGOUT_UART, &uart_config);
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }
    return CODE_SUCCESS;
}


void dbg_printf_uart(const char *__format, ...)
{
    va_list arg;

    memcpy(tx_buffer, 0, BOARD_DBGOUT_BUFFER_SIZE);

    va_start (arg, __format);
    vsnprintf ((char *)tx_buffer,BOARD_DBGOUT_BUFFER_SIZE-1, __format, arg);
    va_end (arg);

    uart_tx(BOARD_DBGOUT_UART, tx_buffer, strlen((char *)tx_buffer), 100);
}