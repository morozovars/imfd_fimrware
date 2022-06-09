#ifndef THREAD_RTOS_TRACE_H_
#define THREAD_RTOS_TRACE_H_


#include "cmsis_os.h"
#include "util/err_codes.h"


void thread_rtos_trace_init(osThreadId_t * p_cur_thread_id);
void thread_rtos_trace_printout(void);
void thread_rtos_trace_run(osThreadId_t * p_thread_ids);


#endif // THREAD_RTOS_THEAD_H_