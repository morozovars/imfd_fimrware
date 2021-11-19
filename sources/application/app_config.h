#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_


#include <stdbool.h>
#include "usbd_cdc.h"


/* Threads config.  */
#define APP_THREAD_RTOS_TRACE_PERIOD_MS           (1u)
#define APP_RTOS_TRACE_PRINTOUT_PERIOD_MS         (60000u)

/* USBD config. */
#define APP_USBD_IRQ_PRIO                         (6u)
#define APP_USBD_BUF_SIZES                        (2048)
#define APP_QUEUE_RX_MSG_COUNT                    (512u)


typedef struct
{
    union {
        uint8_t   buf[CDC_DATA_FS_MAX_PACKET_SIZE];
        double    buf_meas[8];
        }     data;
    uint16_t  length;
} app_queue_rx_msg_t;


#define APP_QUEUE_RX_MSG_SIZE                     (sizeof(app_queue_rx_msg_t))


#endif // APP_CONFIG_H_