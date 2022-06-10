#ifndef THREAD_COMMUNICATION_H_
#define THREAD_COMMUNICATION_H_


#include "cmsis_os2.h"
#include "util/err_codes.h"
#include <stdint.h>


enum
{
    COMMUNICATION_HEADER_TYPE_IDX = 0u,
    COMMUNICATION_HEADER_TYPE_SIZE = 1u,
    COMMUNICATION_HEADER_LEN_IDX = 4u,
    COMMUNICATION_HEADER_LEN_SIZE = 4u,
    COMMUNICATION_HEADER_TOTAL_SIZE = 8u, // 1 byte data type + 3 bytes dummy + 4 bytes length.
};


enum
{
    COMMUNICATION_MSG_TYPE_MEASURE = 0xAA,
    COMMUNICATION_MSG_TYPE_SET_FREQ = 0xAB,
    COMMUNICATION_MSG_TYPE_SET_MEAS_TYPE = 0xAC,

    COMMUNICATION_MSG_TYPE_USE_DEFAULT_REF_GMV = 0xCA,
    COMMUNICATION_MSG_TYPE_USE_CALIB_REF_GMV = 0xCB,
    COMMUNICATION_MSG_TYPE_USE_CUR_GMV_AS_REF_GMV = 0xCC,
    COMMUNICATION_MSG_TYPE_STORE_CUR_GMV_AS_CALIB = 0xCD,
    COMMUNICATION_MSG_TYPE_CHANGE_IFR_OF_DEBUG = 0xDD,
};

enum
{
    COMMUNICATION_MEAS_TYPE_CURRENT = 0u,
    COMMUNICATION_MEAS_TYPE_VIB_VERTICAL = 1u,
    COMMUNICATION_MEAS_TYPE_VIB_AXIAL = 2u,
};


typedef enum
{
    COMMUNICATION_RET_MSG_TYPE_DEBUG_GMV = 0xDF,
    COMMUNICATION_RET_MSG_TYPE_DEBUG_CMPLX_MAG = 0xDC,
    COMMUNICATION_RET_MSG_TYPE_SINGLE_SLOPE = 0xBA,
    COMMUNICATION_RET_MSG_TYPE_DOUBLE_SLOPE = 0xBB,
} communication_ret_msg_type_t;


typedef struct
{
    osMessageQueueId_t * p_queue_tx_msg_id;
    osMessageQueueId_t * p_queue_rx_msg_id;
    osThreadId_t * p_wakeup_thread_id;
    osThreadId_t * p_cur_thread_id;
    uint32_t flags;
    void (*stop_stream_cb)(void);
} thread_communication_init_t;


ret_code_t thread_communication_init(thread_communication_init_t * p_init);
ret_code_t thread_communication_run(void);
ret_code_t thread_communication_transmit(
    communication_ret_msg_type_t type, uint8_t * p_buf, uint16_t len);


#endif // THREAD_COMMUNICATION_H_