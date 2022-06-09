#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_


#include <stdbool.h>
#include "usbd_cdc.h"


/* Threads config.  */
#define APP_THREAD_RTOS_TRACE_PERIOD_MS           (1u)
#define APP_RTOS_TRACE_PRINTOUT_PERIOD_MS         (60000u)

/* USBD config. */
#define APP_USBD_IRQ_PRIO                         (6u)
#define APP_USBD_RX_BUF_SIZE                      (CDC_DATA_FS_MAX_PACKET_SIZE)
#define APP_USBD_TX_BUF_SIZE                      (5120u)
#define APP_QUEUE_RX_MSG_COUNT                    (420u)


/* FLASH addresses with calibrations. */
#define APP_FLASH_ADDR_CALIB_REG_GMV_CURRENT      (0x0807D000)
#define APP_FLASH_ADDR_CALIB_REG_GMV_VIB1         (0x0807E000)
#define APP_FLASH_ADDR_CALIB_REG_GMV_VIB2         (0x0807F000)


typedef struct

{
    uint8_t   buf[CDC_DATA_FS_MAX_PACKET_SIZE];
    uint8_t   length;
} app_queue_rx_msg_t;


#define APP_QUEUE_RX_MSG_SIZE                     (sizeof(app_queue_rx_msg_t))


#endif // APP_CONFIG_H_