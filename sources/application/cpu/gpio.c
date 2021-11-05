/* Includes. */
#include "gpio.h"


/* Private types and defines. */
enum 
{
  GPIO_EXTI_LINE_0  = 0x00, 
  GPIO_EXTI_LINE_1, 
  GPIO_EXTI_LINE_2, 
  GPIO_EXTI_LINE_3, 
  GPIO_EXTI_LINE_4, 
  GPIO_EXTI_LINE_5, 
  GPIO_EXTI_LINE_6, 
  GPIO_EXTI_LINE_7, 
  GPIO_EXTI_LINE_8, 
  GPIO_EXTI_LINE_9, 
  GPIO_EXTI_LINE_10,
  GPIO_EXTI_LINE_11,
  GPIO_EXTI_LINE_12,
  GPIO_EXTI_LINE_13,
  GPIO_EXTI_LINE_14,
  GPIO_EXTI_LINE_15,
  GPIO_EXTI_CNT,
};


/* Private variables. */
static EXTI_HandleTypeDef              hexti_arr[GPIO_EXTI_CNT];
const uint32_t exti_lines_handles[]  = {EXTI_LINE_0,
                                        EXTI_LINE_1,
                                        EXTI_LINE_2,
                                        EXTI_LINE_3,
                                        EXTI_LINE_4,
                                        EXTI_LINE_5,
                                        EXTI_LINE_6,
                                        EXTI_LINE_7,
                                        EXTI_LINE_8,
                                        EXTI_LINE_9,
                                        EXTI_LINE_10,
                                        EXTI_LINE_11,
                                        EXTI_LINE_12,
                                        EXTI_LINE_13,
                                        EXTI_LINE_14,
                                        EXTI_LINE_15};


/* Private inline functions.  */
static inline IRQn_Type pin2irq_type(gpio_pins_t pin)//NOTE: cpu dependent!
{
  uint8_t line_num = pin2line(pin);
  switch (line_num)
  {
    case GPIO_EXTI_LINE_0:
      return EXTI0_IRQn;
    case GPIO_EXTI_LINE_1:
      return EXTI1_IRQn;
    case GPIO_EXTI_LINE_2:
      return EXTI2_IRQn;
    case GPIO_EXTI_LINE_3:
      return EXTI3_IRQn;
    case GPIO_EXTI_LINE_4:
      return EXTI4_IRQn;
    case GPIO_EXTI_LINE_5:
    case GPIO_EXTI_LINE_6:
    case GPIO_EXTI_LINE_7:
    case GPIO_EXTI_LINE_8:
    case GPIO_EXTI_LINE_9:
      return EXTI9_5_IRQn;
    case GPIO_EXTI_LINE_10:
    case GPIO_EXTI_LINE_11:
    case GPIO_EXTI_LINE_12:
    case GPIO_EXTI_LINE_13:
    case GPIO_EXTI_LINE_14:
    case GPIO_EXTI_LINE_15:
      return EXTI15_10_IRQn;
    default:
      break;
  }
  return HardFault_IRQn;
}


/* Public functions. */
void gpio_pin_cfg_output(gpio_pins_t pin, gpio_cfg_out_t * cfg)
{
  GPIO_TypeDef * p_port = pin2port(pin);
  GPIO_InitTypeDef gpio_init = {
    .Mode = (uint32_t)cfg->type,
    .Pin = pin2pin(pin),
    .Speed = (uint32_t)cfg->frequency,
    .Pull = (uint32_t)cfg->pull};

  port_enable(pin);
  HAL_GPIO_Init(p_port, &gpio_init);
}


void gpio_pin_cfg_input(gpio_pins_t pin, gpio_cfg_in_t * cfg)
{
  GPIO_InitTypeDef gpio_init = {
    .Mode = (uint32_t)cfg->type,
    .Pin = pin2pin(pin),
    .Speed = (uint32_t)cfg->frequency,
    .Pull = (uint32_t)cfg->pull};
  uint8_t line_num = pin2line(pin);
  IRQn_Type irq_type = pin2irq_type(pin);

  port_enable(pin);
  HAL_GPIO_Init(pin2port(pin), &gpio_init);

  if (cfg->type == GPIO_IN_DEFAULT)
  {
    return;
  }
  HAL_EXTI_GetHandle(&hexti_arr[line_num], exti_lines_handles[line_num]);  
  HAL_NVIC_SetPriority(irq_type, cfg->prio, 0x00);
  HAL_NVIC_EnableIRQ(irq_type);
}


void gpio_pin_cfg_af(gpio_pins_t pin, gpio_cfg_af_t * cfg)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = pin2pin(pin);
    GPIO_InitStruct.Mode = cfg->type;
    GPIO_InitStruct.Pull = cfg->pull;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = cfg->assign_io_mux;
    
    port_enable(pin);
    HAL_GPIO_Init(pin2port(pin),  &GPIO_InitStruct);
}


void gpio_pin_cfg_register_cb(gpio_pins_t pin, gpio_cb_t * p_cb)
{
  uint8_t line_num = pin2line(pin);
  HAL_EXTI_RegisterCallback(&hexti_arr[line_num], HAL_EXTI_COMMON_CB_ID, (void *)p_cb);
  /* note:  return code of HAL_EXTI_RegisterCallback not handled, because
            it could return error only in case if 2nd arguemtn not equal
            to HAL_EXTI_COMMON_CB_ID. In our case it always have this value.
   */
}


void gpio_pin_set(gpio_pins_t pin)
{
  HAL_GPIO_WritePin(pin2port(pin), pin2pin(pin), GPIO_PIN_SET);
}


void gpio_pin_clean(gpio_pins_t pin)
{
  HAL_GPIO_WritePin(pin2port(pin), pin2pin(pin), GPIO_PIN_RESET);
}


void gpio_pin_toggle(gpio_pins_t pin)
{
  HAL_GPIO_TogglePin(pin2port(pin), pin2pin(pin));
}


gpio_state_t gpio_pin_read(gpio_pins_t pin)
{
  return (gpio_state_t)HAL_GPIO_ReadPin(pin2port(pin), pin2pin(pin));
}


void gpio_pin_remove_cfg(gpio_pins_t pin)
{
  HAL_GPIO_DeInit(pin2port(pin), pin2pin(pin));
}


/* CPU's EXTI's interrupt handlers. */
void EXTI0_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_0]);
}


void EXTI1_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_1]);
}


void EXTI2_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_2]);
}


void EXTI3_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_3]);
}


void EXTI4_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_4]);
}


void EXTI9_5_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_5]);
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_6]);
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_7]);
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_8]);
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_9]);
}


void EXTI15_10_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_10]);
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_11]);
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_12]);
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_13]);
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_14]);
  HAL_EXTI_IRQHandler(&hexti_arr[GPIO_EXTI_LINE_15]);
}