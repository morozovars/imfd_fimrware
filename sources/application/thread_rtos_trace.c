#include "thread_rtos_trace.h"
#include "app_config.h"
#include "trace/app_trace.h"

enum
{
    FLAG_PRINTOUT = 0x01,
};


static osThreadId_t * p_thread_id;

void thread_rtos_trace_init(osThreadId_t * p_cur_thread_id)
{
    p_thread_id = p_cur_thread_id;
    app_trace_init();
}

void thread_rtos_trace_printout(void) { osThreadFlagsSet(*p_thread_id, FLAG_PRINTOUT); }

void thread_rtos_trace_run(osThreadId_t * p_thread_ids)
{
    uint32_t flags = osThreadFlagsWait(FLAG_PRINTOUT, osFlagsWaitAny, osWaitForever);
    if (flags == FLAG_PRINTOUT)
    {
        app_trace_print_timestat();
        app_trace_print_stackheap_usage((void *)p_thread_ids, THREADS_CNT);
    }
}