#ifndef APP_TRACE_H_
#define APP_TRACE_H_


#if (defined(APP_TRACE) && defined(USE_HAL_DRIVER))


#include <stdint.h>
#include <stdio.h>                  //For work with printf.
#include "stm32g4xx_hal.h"          //TODO: should be modified for your CPU. For work with timer.
#include "util/err_codes.h"


//TODO: CONFIGURE MODULE:
/* MODULE PARAMETERS. */
#define APP_TRACE_TASKS_CNT     (2)
#define APP_TRACE_PRINTF        (printf)
#define APP_TRACE_EXTERN_TIM    (1)
#if (APP_TRACE_EXTERN_TIM == 0)
# define APP_TRACE_TIMER_PRIO   (5)       // Timer Interrupt Priority (not tested with other values).
# define APP_TRACE_TIMER_FREQ   (1000000) // Hz (not tested with others frequencies).
#else
# define APP_TRACE_TIMER_FREQ   (1000000)
  extern TIM_HandleTypeDef      htim6;    //WARNING: select your timer.
# define APP_TRACE_TIMER        (&htim6)  //NOTE: initialized in stm32g4xx_hal_timebase_tim.c
#endif


/**@brief: Initalize trace timer.
*/
void app_trace_init(void);


/**@brief:  Get current time. If TRACE_RTOS is defined then this function should be
 *          used in FreeRTOSConfig.h for portGET_RUN_TIME_COUNTER_VALUE().
 *
 * @param[out]    measured time in microseconds.
*/
uint32_t app_trace_get_time_us(void);


/**@brief:  Calculate determine how much time has passed between previously measured
 *          time and current.
 *
 * @param[in]     previously measured time (in microseconds).
 *
 * @param[out]    difference between current time and t_old, in microseconds.
*/
uint32_t app_trace_get_time_diff(uint32_t t_old);


/**@brief:  The same as app_trace_get_time_diff(), but also update t_old.
 *
 * @param[in]     pointer on previously measured time (in microseconds).
 *
 * @param[out]    difference between current time and t_old, in microseconds.
*/
uint32_t app_trace_time_step(uint32_t * p_t_old);


# if (defined(TRACE_RTOS))
  /**@brief:  Function prints timestat according to format specified - 
  *           https://www.freertos.org/rtos-run-time-stats.html.
  */
  void app_trace_print_timestat(void);
  
  
  /**@brief:  Function prints stack high watermark for each thread, and minimum ever free
  *           heap size.
  *
  * @param[in]    Array of  the threads ids (handles), which needs
  *               heep usage analysis.
  * @param[in]    Count of threads in threads[].
  */
  void app_trace_print_stackheap_usage(const void * threads[], unsigned int cnt);  
# else //!TRACE_RTOS

  /**@brief:  Dummy for current configuration
  *
  */
  static inline void app_trace_print_timestat(void) { ; }


  /**@brief:  Dummy for current configuration
  *
  */
  static inline void app_trace_print_stackheap_usage(const void * threads[], unsigned int cnt)
  {(void)threads; (void)cnt;}
# endif //TRACE_RTOS


#endif //(defined(APP_TRACE) && defined(USE_HAL_DRIVER))


#endif //APP_TRACE_H_