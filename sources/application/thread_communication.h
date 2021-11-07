#ifndef THREAD_COMMUNICATION_H_
#define THREAD_COMMUNICATION_H_


#include <stdint.h>
#include "util/err_codes.h"
#include "cmsis_os2.h"


typedef struct
{
    osMessageQueueId_t *      p_queue_id;
    osThreadId_t  *           p_wakeup_thread_id;
    uint32_t                  flags;
}thread_communication_init_t;


ret_code_t thread_communication_init(thread_communication_init_t * p_init);
ret_code_t thread_communication_run(void);
ret_code_t thread_communication_transmit(uint8_t * p_buf, uint16_t len);


#endif // THREAD_COMMUNICATION_H_