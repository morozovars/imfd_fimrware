#ifndef THREAD_COMMUNICATION_H_
#define THREAD_COMMUNICATION_H_


#include <stdint.h>
#include "util/err_codes.h"
#include "cmsis_os2.h"


enum {
    COMMUNICATION_DATA_TYPE_MEASURE       = 0xAA,
    COMMUNICATION_DATA_TYPE_SET_FREQ      = 0xAB,
    COMMUNICATION_DATA_TYPE_SET_MEAS_TYPE = 0xAC,
};

enum {
    COMMUNICATION_HEADER_LENGTH           = 8u, // 1 byte data type + 3 bytes dummy + 4 bytes length.
};

enum {
    COMMUNICATION_MEAS_TYPE_CURRENT =       0u,
    COMMUNICATION_MEAS_TYPE_VIB_VERTICAL =  1u,
    COMMUNICATION_MEAS_TYPE_VIB_AXIAL =     2u,
};


typedef struct
{
    osMessageQueueId_t *      p_queue_msg_id;
    osThreadId_t  *           p_wakeup_thread_id;
    osThreadId_t  *           p_cur_thread_id;
    uint32_t                  flags;
}thread_communication_init_t;


ret_code_t thread_communication_init(thread_communication_init_t * p_init);
ret_code_t thread_communication_run(void);
ret_code_t thread_communication_transmit(uint8_t * p_buf, uint16_t len);


#endif // THREAD_COMMUNICATION_H_