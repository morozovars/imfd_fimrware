#ifndef B_G474E_DPOW1_H_
#define B_G474E_DPOW1_H_


#include <stdint.h>
#include "stm32g4xx_hal.h"


typedef enum
{
  LED2 = 0U,
  LED_BLUE = LED2,
  LED3 = 1U,
  LED_ORANGE = LED3,
  LED4 = 2U,
  LED_GREEN = LED4,
  LED5 = 3U,
  LED_RED = LED5,
  LED_CNT = 4U,
} bsp_led_t;


typedef enum
{
  JOY_SEL,
  JOY_DOWN,
  JOY_LEFT,
  JOY_RIGHT,
  JOY_UP,
  JOY_CNT,
} bsp_joy_t;


typedef void (* bsp_cb_t)(bsp_joy_t joy, GPIO_PinState joy_state);


void bsp_init(void);
void bsp_led_on(bsp_led_t led);
void bsp_led_off(bsp_led_t led);
void bsp_led_toggle(bsp_led_t led);
uint8_t bsp_joy_get_state(bsp_joy_t joy);
void bsp_joy_register_cb(bsp_cb_t cb);
void bsp_uninit(void);


#endif