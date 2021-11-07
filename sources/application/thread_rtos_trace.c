#include "thread_rtos_trace.h"
#include "trace/app_trace.h"


void thread_rtos_trace_init(void)
{
    app_trace_init();
}


void thread_rtos_trace_run(osThreadId_t * p_thread_ids)
{
    app_trace_print_timestat();
    app_trace_print_stackheap_usage((void *)p_thread_ids, THREADS_CNT);
}