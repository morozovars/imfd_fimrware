#ifndef THREAD_RTOS_TRACE_H_
#define THREAD_RTOS_TRACE_H_


#include "cmsis_os.h"
#include "util/err_codes.h"


void thread_rtos_trace_init(void);
void thread_rtos_trace_run(osThreadId_t * p_thread_ids, uint32_t tick_cur);


#endif // THREAD_RTOS_THEAD_H_