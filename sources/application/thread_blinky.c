#include "thread_blinky.h"
#include "bsp/b-g474e-dpow1.h"
#include "cmsis_os.h"
#include "trace/dbgout.h"
#include "thread_rtos_trace.h"


static void bsp_joy_cb(bsp_joy_t joy, GPIO_PinState joy_state)
{
  switch (joy)
  {
    case JOY_SEL:    
      dbg_printf("JOY: sel pressed!\n");
      thread_rtos_trace_printout();
      break;
    case JOY_UP:
      dbg_printf("JOY: up pressed!\n");
      break;
    case JOY_DOWN:
      dbg_printf("JOY: down pressed!\n");
      break;
    case JOY_RIGHT:
      dbg_printf("JOY: right pressed!\n");
      break;
    case JOY_LEFT:
      dbg_printf("JOY: left pressed!\n");
      break;
    default:
      break;
  }
  (void) joy;
  (void) joy_state;
}


void thread_blinky_init(void)
{
  bsp_init();
  bsp_joy_register_cb(bsp_joy_cb);
}


void thread_blinky_run(void)
{
  bsp_led_toggle(LED_BLUE);
  osDelay(200);
  bsp_led_toggle(LED_ORANGE);
  osDelay(200);
  bsp_led_toggle(LED_RED);
  osDelay(200);
  bsp_led_toggle(LED_GREEN);
  osDelay(200);
}