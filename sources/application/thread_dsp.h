#ifndef THREAD_DSP_H_
#define THREAD_DSP_H_


#include "cmsis_os2.h"
#include "util/err_codes.h"


#define THREAD_DSP_WAKEUP_FLAG        0x00000001

#define THREAD_MEAS_SIZE              sizeof(double)

typedef struct 
{
    osMessageQueueId_t *      p_new_data_queue;
} thread_dsp_init_t;


ret_code_t thread_dsp_init(thread_dsp_init_t * p_init);


ret_code_t thread_dsp_run(void);


void thread_dsp_stop_meas_stream_cb(void);


#endif // THREAD_DSP_H_