#include "thread_rtos_trace.h"
#include "trace/app_trace.h"
#include "app_config.h"


static uint32_t start_tick;
static uint32_t dt;


void thread_rtos_trace_init(void)
{
    app_trace_init();
    start_tick = 0;
}


void thread_rtos_trace_run(osThreadId_t * p_thread_ids, uint32_t tick_cur)
{    
    /// Calculate dt.
    if (tick_cur > start_tick)
    {
        dt = tick_cur - start_tick;
    }
    else
    {
        dt = tick_cur + (UINT32_MAX -start_tick) + 1;
    }

    /// Check if required_delay not expired.
    if (dt < APP_RTOS_TRACE_PRINTOUT_PERIOD_MS)
    {
        return CODE_SUCCESS;
    }

    app_trace_print_timestat();
    app_trace_print_stackheap_usage((void *)p_thread_ids, THREADS_CNT);
    start_tick = tick_cur;
}