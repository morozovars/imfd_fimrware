#include "app_trace.h"
#include "FreeRTOS.h"
#include "task.h"


#if (defined(APP_TRACE) && defined(USE_HAL_DRIVER))


#if (APP_TRACE_EXTERN_TIM == 0)

//TODO: put your timer
extern TIM_HandleTypeDef                htim7;
TIM_HandleTypeDef * p_trace_timer =     &htim7;
static uint32_t app_trace_timer_count = 0;


static void trace_timer_PeriodElapsedCallback(TIM_HandleTypeDef *htim);


void app_trace_init(void)
{

  /* Initialize timer for tracing.  */
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock = 0;
  uint32_t              uwAPB1Prescaler;
  uint32_t              uwPrescalerValue = 0;
  uint32_t              pFLatency;
  HAL_StatusTypeDef     status = HAL_OK;

  __HAL_RCC_TIM7_CLK_ENABLE();

  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
  uwAPB1Prescaler = clkconfig.APB1CLKDivider;
  uwTimclock = HAL_RCC_GetPCLK1Freq();
  if (uwAPB1Prescaler != RCC_HCLK_DIV1)
  {
    uwTimclock = uwTimclock * 2U;
  }

  /* Compute the prescaler value to have TIM7 counter clock equal to APP_TRACE_TIMER_FREQ Hz. */
  uwPrescalerValue = (uint32_t)((uwTimclock / APP_TRACE_TIMER_FREQ) - 1U);

  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM7CLK/1000) - 1]. to have a 1 kHz s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a APP_TRACE_TIMER_FREQ Hz counter clock.
  + ClockDivision = 0.
  + Counter direction = Up.
  */
  p_trace_timer->Instance = TIM7;
  p_trace_timer->Init.Period = (APP_TRACE_TIMER_FREQ / 1000) - 1;
  p_trace_timer->Init.Prescaler = uwPrescalerValue;
  p_trace_timer->Init.ClockDivision = 0;
  p_trace_timer->Init.CounterMode = TIM_COUNTERMODE_UP;
  status = HAL_TIM_Base_Init(p_trace_timer);
  if (status != HAL_OK)
  {
      APP_TRACE_PRINTF("RTOS Debug timer init failed\n");
      return;
  }
  
  HAL_TIM_RegisterCallback(p_trace_timer, HAL_TIM_PERIOD_ELAPSED_CB_ID, trace_timer_PeriodElapsedCallback);

  status = HAL_TIM_Base_Start_IT(p_trace_timer);
  if (status != HAL_OK)
  {
      APP_TRACE_PRINTF("RTOS Debug timer init failed\n");
      return;
  }

  HAL_NVIC_EnableIRQ(TIM7_DAC_IRQn);
  
  if (APP_TRACE_TIMER_PRIO < (1UL << __NVIC_PRIO_BITS))
  {
    HAL_NVIC_SetPriority(TIM7_DAC_IRQn, APP_TRACE_TIMER_PRIO, 0U);
    uwTickPrio = APP_TRACE_TIMER_PRIO;
  }
}


static void trace_timer_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == p_trace_timer)
  {
    app_trace_timer_count++;
  }
}


static inline uint32_t get_time_us(void)
{
  enum { cnt_us = (1000000/APP_TRACE_TIMER_FREQ), tick_us = 1000};	/* HAL GetTick -> ms, Timer -> us */
  
  uint32_t tc1 = app_trace_timer_count,
           cnt = __HAL_TIM_GET_COUNTER(p_trace_timer),
           tc2 = app_trace_timer_count;
  
  if (tc1 != tc2) { cnt = __HAL_TIM_GET_COUNTER(p_trace_timer); }	/* Interrupt during GET_COUNTER */
  
  return (uint32_t)((uint64_t)tc2 * tick_us + cnt * cnt_us);
}


#else //(APP_TRACE_EXTERN_TIM == 1)


void app_trace_init(void)
{
    ;//initialized in stm32xxx_hal_timebase_tim.c
}


static inline uint32_t get_time_us(void)
{
  enum { cnt_us = (1000000/APP_TRACE_TIMER_FREQ), tick_us = 1000};	/* HAL GetTick -> ms, Timer -> us */
  
  uint32_t tc1 = HAL_GetTick(),
           cnt = __HAL_TIM_GET_COUNTER(APP_TRACE_TIMER),
           tc2 = HAL_GetTick();
  
  if (tc1 != tc2) { cnt = __HAL_TIM_GET_COUNTER(APP_TRACE_TIMER); }	/* Interrupt during GET_COUNTER */
  
  return (uint32_t)((uint64_t)tc2 * tick_us + cnt * cnt_us);
}


#endif //defined(APP_TRACE_EXTERN_TIM)


uint32_t app_trace_get_time_us(void)
{
  return get_time_us();
}


uint32_t app_trace_get_time_diff(uint32_t t_old)
{
  uint32_t t_curr = get_time_us();
  
  if (t_curr >= t_old) {
    return t_curr - t_old;
  }

  return t_curr + (UINT32_MAX - t_old) + 1;	/* Time Overflow */
}


uint32_t app_trace_time_step(uint32_t * p_t_old)
{
  uint32_t t_curr = get_time_us(), diff = *p_t_old;
  
  if (t_curr >= diff) {
    diff = t_curr - diff;
  } else {
    diff = t_curr + (UINT32_MAX - diff) + 1;	/* Time Overflow */
  }

  *p_t_old = t_curr;
  
  return diff;
}


# if (defined(TRACE_RTOS))
void app_trace_print_timestat(void)
{
  static char write_out_buf[APP_TRACE_TASKS_CNT * 40];
  write_out_buf[0] = 0;
  vTaskGetRunTimeStats(write_out_buf);
  APP_TRACE_PRINTF("Threads stat:\n%s", (write_out_buf[0] != 0) ? write_out_buf : "?\n");
}


void app_trace_print_stackheap_usage(const void * threads[], unsigned int cnt)
{
  const TaskHandle_t * tasks = (TaskHandle_t * )threads;
  APP_TRACE_PRINTF("Stack Bytes left:\n");
  for (uint32_t i = 0; i < cnt; i++) {
    APP_TRACE_PRINTF("%s - %d\n", pcTaskGetName(tasks[i]), uxTaskGetStackHighWaterMark(tasks[i]) * sizeof(StackType_t));
  }
  APP_TRACE_PRINTF("Heap  Bytes left:  %u\n", (uint32_t)xPortGetMinimumEverFreeHeapSize());
}
# endif


#endif
