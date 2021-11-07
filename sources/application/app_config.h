#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_


#include <stdbool.h>


/* Threads config.  */
#define APP_THREAD_RTOS_TRACE_PERIOD_MS           (10000u)

/* USBD config. */
#define APP_USBD_IRQ_PRIO                         (6u)
#define APP_USBD_BUF_SIZES                        (2048)
#define APP_QUEUE_RX_MSG_COUNT                    (5)
typedef struct
{
    uint8_t   buf[APP_USBD_BUF_SIZES];
    uint16_t  length;
    bool      overflow;
} app_queue_rx_msg_t;
#define APP_QUEUE_RX_MSG_SIZE                     (sizeof(app_queue_rx_msg_t))

#endif // APP_CONFIG_H_