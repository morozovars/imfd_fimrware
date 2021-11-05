#include "b-g474e-dpow1.h"
#include "gpio.h"
#include <stdio.h>


#define LED2_GPIO_PIN     PA15
#define LED3_GPIO_PIN     PB1
#define LED4_GPIO_PIN     PB7
#define LED5_GPIO_PIN     PB5

#define JOY_SEL_GPIO_PIN          PC13
#define JOY_UP_GPIO_PIN           PB10
#define JOY_DOWN_GPIO_PIN         PC5
#define JOY_RIGHT_GPIO_PIN        PB2
#define JOY_LEFT_GPIO_PIN         PC4


static const uint32_t led_pins[LED_CNT] = {LED2_GPIO_PIN,
                                          LED3_GPIO_PIN,
                                          LED4_GPIO_PIN,
                                          LED5_GPIO_PIN};


static const uint32_t joy_pins[JOY_CNT] = {JOY_SEL_GPIO_PIN,
                                           JOY_UP_GPIO_PIN,
                                           JOY_DOWN_GPIO_PIN,
                                           JOY_RIGHT_GPIO_PIN,
                                           JOY_LEFT_GPIO_PIN};


static bsp_cb_t p_bsp_cb;


static void SEL_JOY_EXTI_Callback(void)
{
  if (p_bsp_cb != NULL)
  {
    p_bsp_cb(JOY_SEL, GPIO_PIN_SET);
  }
}


static void UP_JOY_EXTI_Callback(void)
{
  if (p_bsp_cb != NULL)
  {
    p_bsp_cb(JOY_UP, GPIO_PIN_SET);
  }
}


static void DOWN_JOY_EXTI_Callback(void)
{
  if (p_bsp_cb != NULL)
  {
    p_bsp_cb(JOY_DOWN, GPIO_PIN_SET);
  }
}


static void RIGHT_JOY_EXTI_Callback(void)
{
  if (p_bsp_cb != NULL)
  {
    p_bsp_cb(JOY_RIGHT, GPIO_PIN_SET);
  }
}


static void LEFT_JOY_EXTI_Callback(void)
{
  if (p_bsp_cb != NULL)
  {
    p_bsp_cb(JOY_LEFT, GPIO_PIN_SET);
  }
}


void bsp_init(void)
{
  static void * joy_cbs[JOY_CNT] = {
   SEL_JOY_EXTI_Callback, UP_JOY_EXTI_Callback, DOWN_JOY_EXTI_Callback,
   RIGHT_JOY_EXTI_Callback, LEFT_JOY_EXTI_Callback};

  /* Configure the LEDs pin */
  gpio_cfg_out_t pin_out_cfg = {
    .frequency = GPIO_FREQ_LOW,
    .pull = GPIO_PULL_UP,
    .type = GPIO_OUTPUT_PUSH_PULL};
  for (uint8_t i = 0; i < LED_CNT; i++)
  {
    gpio_pin_cfg_output(led_pins[i], &pin_out_cfg);
    gpio_pin_set(led_pins[i]);
  }
  
  /* Configure the JOYSTICKs pin */
  gpio_cfg_in_t pin_in_cfg = {
    .frequency = GPIO_FREQ_LOW,
    .pull = GPIO_PULL_UP,
    .type = GPIO_IN_IT_FALLING,
    .prio = 0x03};
  for (uint8_t i = 0; i < JOY_CNT; i++)
  {
    gpio_pin_cfg_input(joy_pins[i], &pin_in_cfg);
    gpio_pin_cfg_register_cb(joy_pins[i], joy_cbs[i]);
  }
}


void bsp_led_on(bsp_led_t led)
{
  gpio_pin_set(led_pins[led]);
}


void bsp_led_off(bsp_led_t led)
{
  gpio_pin_clean(led_pins[led]);
}


void bsp_led_toggle(bsp_led_t led)
{
  gpio_pin_toggle(led_pins[led]);
}


uint8_t bsp_joy_get_state(bsp_joy_t joy)
{
  return (gpio_state_t)gpio_pin_read(joy_pins[joy]);
}


void bsp_joy_register_cb(bsp_cb_t cb)
{
  p_bsp_cb = cb;
}

void bsp_uninit(void)
{
  ;
}