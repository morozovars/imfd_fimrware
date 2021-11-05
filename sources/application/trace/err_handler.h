#ifndef ERR_HANDLER_H_
#define ERR_HANDLER_H_


#include <stdbool.h>
#include "dbgout.h"
#include "cmsis_os.h"


//TODO: CONFIGURE MODULE:
/* MODULE PARAMETERS. */
#define ERR_HANDLER_PRINTF        dbg_printf
#define ERR_HANDLER_TAG           "<Err>"


/* List of supported errors.  */
typedef enum
{
  sys_ok          = 0u,                 /*  No errors.  */
  sys_app_thread  = 1u,                 /*  Error occured in thread.  */
  sys_assert      = sys_app_thread * 2, /*  STM32xxx HAL assert.  */
  sys_stack       = sys_assert * 2,     /*  FreeRTOS stack overflow hook.  */
  sys_malloc      = sys_stack * 2,      /*  FreeRTOS malloc failed hook.  */
} sys_fault_t;


/**@brief:  Check if any error occurs in system.
 *
 * @param[out]    false - no errors, true - any error occured.
*/
bool err_check(void);


/**@brief:  Register error in system.
 *
 * @param[in]     type of occured error.
 *
 */
void err_set(sys_fault_t error);


/**@brief:  Return occured error.
 *
 * @param[out]    type of occured error.
*/
sys_fault_t err_get(void);


/**@brief:  Print info about occured error.
 *
*/
void err_printout(void);


/**@brief:  Clear registered error.
 *
*/
void err_clear(void);


/**@brief:  FreeRTOS hook of stack overflow. For details -
 *          https://www.freertos.org/Stacks-and-stack-overflow-checking.html
 *
*/
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);


/**@brief:  FreeRTOS hook of malloc failure. For details -
 *          https://www.freertos.org/a00016.html
 *
*/
void vApplicationMallocFailedHook(void);


#endif //ERR_HANDLER_H_