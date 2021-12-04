#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_


#include "gpio.h"
#include "uart.h"
#include "app_config.h"


/* Communication USB. */
#define BOARD_COMMUNICATION_DP_PIN          (PA12)
#define BOARD_COMMUNICATION_DM_PIN          (PA11)


/* DBGOUT UART. */
#define BOARD_DBGOUT_UART                   (UART_INST_3)
#define BOARD_DBGOUT_UART_SPEED             (115200)
#define BOARD_DBGOUT_UART_INT_PRIO          (UART_WITHOUT_IT)
#define BOARD_DBGOUT_UART_TX_AF             (GPIO_AF7_USART3)
#define BOARD_DBGOUT_UART_RX_AF             (GPIO_AF7_USART3)
#define BOARD_DBGOUT_UART_PIN_TX            (PC10)
#define BOARD_DBGOUT_UART_PIN_RX            (PC11)
#define BOARD_DBGOUT_BUFFER_SIZE            (512u)


#endif //BOARD_CONFIG_H_