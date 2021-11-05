#	App Trace Template

Template of module, which provide a couple of functions to trace your application with (or without) FreeRTOS.

##	1) Module purposes:

This Module provide functions for FreeRTOS debugging and for measuring time between events. For FreeRTOS, it checks how many CPU time were consumed by each task\thread, and how many bytes left in stack heap of each task\thread. Also it provide  This module usedn FreeRTOS native functions: vTaskGetRunTimeStats(), uxTaskGetStackHighWaterMark() and xPortGetMinimumEverFreeHeapSize(). This module require one timer for measuring time. It could be shared with other modules. Frequency of this timer should be higher then RTOS's timer at least in 10 times.
      
##	2) Module configuration:
**Requirements:**
          
- only stm32 (currently);
- usage of stm32 HAL library (currently);
- preprocessor definitions: APP_TRACE and TRACE_RTOS (optionaly);
- 1 shared or separate timer.
- include app_trace.c in your project.

**Settings**

**a)** To use this Module following defines must be set macro "configUSE_STATS_FORMATTING_FUNCTIONS 1" in FreeRTOSConfig.h.
**b)** In FreeRTOSConfig.h, you have to assign some functions from this file to FreeRTOS's macros:
 
```C
#define configUSE_STATS_FORMATTING_FUNCTIONS 			1
extern uint32_t app_trace_get_time_us(); 
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()		;
#define portGET_RUN_TIME_COUNTER_VALUE()                app_trace_get_time_us()
```

**c)** Adjust includes with respect to your CPU (See "TODO:").
 

**d)** Fill MODULE PARAMETERS (See "TODO:") according to your needs:

- set number of tasks\threads in your system - APP_TRACE_TASKS_CNT;
- set your function of printing results (should be printf-like function) - APP_TRACE_PRINTF;
- if you want to use timer which shared with other file(s), define APP_TRACE_EXTERN_TIM 1. But, you have to be sure, that this timer have a suitable frequency (usually it's 1 MHz). Otherwise
- set APP_TRACE_EXTERN_TIM 0 and provide your own initialization in app_trace.c file in app_trace_init() function. Currently it done for TIM7.

**e)** Specify frequency of your timer - in macro APP_TRACE_TIMER_FREQ.
 
##	3) Module usage:
Use functions from this module in places where you want to obtain:
- time statistics of your system (use function app_trace_print_timestat() for that proposes);
- stack usage information (use app_trace_print_stackheap_usage() for that).
Recommended not to use this more often that 1 time per second. Good practice to use 1 separted thread only for that purposes.
- measure time between events in your application (using app_trace_get_time_us(), app_trace_get_time_diff(), app_trace_time_step()).
