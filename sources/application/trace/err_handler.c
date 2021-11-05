#include "err_handler.h"


static volatile sys_fault_t sys_fault = sys_ok;
static volatile osThreadId_t thread_id;


bool err_check(void)
{
  return (sys_fault != sys_ok);
}


void err_set(sys_fault_t error)
{
  sys_fault = error;
  if (error == sys_app_thread)
  {
      thread_id = osThreadGetId();
  }

}


sys_fault_t err_get(void)
{
  return sys_fault;
}


void err_printout(void)
{
  switch (sys_fault)
  {
    case sys_ok:
      ERR_HANDLER_PRINTF("%s no errors\n", ERR_HANDLER_TAG);
      break;
    case sys_app_thread:
      ERR_HANDLER_PRINTF("%s thread error - %s\n", ERR_HANDLER_TAG, osThreadGetName(thread_id));
      break;
    case sys_assert:
      ERR_HANDLER_PRINTF("%s HAL assert occurs\n", ERR_HANDLER_TAG);
      break;
    case sys_stack:
      ERR_HANDLER_PRINTF("%s FreeRTOS stack overflow occurs\n", ERR_HANDLER_TAG);
      break;
    case sys_malloc:
      ERR_HANDLER_PRINTF("%s malloc failed\n", ERR_HANDLER_TAG);
      break;
    default:
      ERR_HANDLER_PRINTF("%s unknown error, mb array out overlapped \"sys_fault\" variable\n", ERR_HANDLER_TAG);
      break;
  }
}


void err_clear(void)
{
  sys_fault = sys_ok;
}


void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
  if (sys_fault & sys_stack)
  {
    return; //error already registered
  }
  sys_fault += sys_stack;
  ERR_HANDLER_PRINTF("%s: Stack %s\n", (char *)ERR_HANDLER_TAG, pcTaskName);
  (void)xTask;
  (void)pcTaskName;
}


void vApplicationMallocFailedHook(void)
{
  if (sys_fault & sys_malloc)
  {
    return; //error already registered
  }
  sys_fault += sys_malloc;
  ERR_HANDLER_PRINTF("%s: Malloc\n", ERR_HANDLER_TAG);
}


#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  if (sys_fault & sys_assert)
  {
  	return;
  }
  sys_fault += sys_assert;
  ERR_HANDLER_PRINTF("%s: Assert %s Line %d\n", ERR_HANDLER_TAG, file, line);
  (void)file;
  (void)line;
}
#endif