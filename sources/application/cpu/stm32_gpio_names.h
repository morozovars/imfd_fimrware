#ifndef STM32_GPIO_NAMES_H
#define STM32_GPIO_NAMES_H


/* Includes. */
#include "stm32g4xx_hal.h"

/**
  * @brief   GPIO names with respect to names of pins on schematic.
  */

#define CS_Test_Pin GPIO_PIN_0
#define CS_Test_GPIO_Port GPIOB
#define ADC_CS_Pin GPIO_PIN_2
#define ADC_CS_GPIO_Port GPIOA
#define ADC_CS1_Pin GPIO_PIN_4
#define ADC_CS1_GPIO_Port GPIOA
#define ADC_sync1_Pin GPIO_PIN_4
#define ADC_sync1_GPIO_Port GPIOC
#define ADC_PSW1_Pin GPIO_PIN_5
#define ADC_PSW1_GPIO_Port GPIOC
#define ADC_PSW_Pin GPIO_PIN_11
#define ADC_PSW_GPIO_Port GPIOB
#define Driver_Fault_Pin GPIO_PIN_9
#define Driver_Fault_GPIO_Port GPIOA
#define Driver_OTW_Pin GPIO_PIN_10
#define Driver_OTW_GPIO_Port GPIOA
#define Driver_OTW_EXTI_IRQn EXTI15_10_IRQn
#define Driver_reset_AB_Pin GPIO_PIN_11
#define Driver_reset_AB_GPIO_Port GPIOA
#define Driver_reset_CD_Pin GPIO_PIN_12
#define Driver_reset_CD_GPIO_Port GPIOA
#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define T_SWO_Pin GPIO_PIN_3
#define T_SWO_GPIO_Port GPIOB


/* GPIO Types. */
/**
  * @brief   GPIO names with respect to names of pins.
  */
typedef enum 
{
    //GPIOA
    PA0   = 0x00,
    PA1   = 0x01,
    PA2   = 0x02,
    PA3   = 0x03,
    PA4   = 0x04,
    PA5   = 0x05,
    PA6   = 0x06,
    PA7   = 0x07,
    PA8   = 0x08,
    PA9   = 0x09,
    PA10  = 0x0A,
    PA11  = 0x0B,
    PA12  = 0x0C,
    PA13  = 0x0D,
    PA14  = 0x0E,
    PA15  = 0x0F,
    //GPIOB
    PB0   = 0x10,
    PB1   = 0x11,
    PB2   = 0x12,
    PB3   = 0x13,
    PB4   = 0x14,
    PB5   = 0x15,
    PB6   = 0x16,
    PB7   = 0x17,
    PB8   = 0x18,
    PB9   = 0x19,
    PB10  = 0x1A,
    PB11  = 0x1B,
    PB12  = 0x1C,
    PB13  = 0x1D,
    PB14  = 0x1E,
    PB15  = 0x1F,
    //GPIOC
    PC0   = 0x20,
    PC1   = 0x21,
    PC2   = 0x22,
    PC3   = 0x23,
    PC4   = 0x24,
    PC5   = 0x25,
    PC6   = 0x26,
    PC7   = 0x27,
    PC8   = 0x28,
    PC9   = 0x29,
    PC10  = 0x2A,
    PC11  = 0x2B,
    PC12  = 0x2C,
    PC13  = 0x2D,
    PC14  = 0x2E,
    PC15  = 0x2F,
    //GPIOD
    PD0   = 0x30,
    PD1   = 0x31,
    PD2   = 0x32,
    PD3   = 0x33,
    PD4   = 0x34,
    PD5   = 0x35,
    PD6   = 0x36,
    PD7   = 0x37,
    PD8   = 0x38,
    PD9   = 0x39,
    PD10  = 0x3A,
    PD11  = 0x3B,
    PD12  = 0x3C,
    PD13  = 0x3D,
    PD14  = 0x3E,
    PD15  = 0x3F,
    //GPIOE
    PE0   = 0x40,
    PE1   = 0x41,
    PE2   = 0x42,
    PE3   = 0x43,
    PE4   = 0x44,
    PE5   = 0x45,
    PE6   = 0x46,
    PE7   = 0x47,
    PE8   = 0x48,
    PE9   = 0x49,
    PE10  = 0x4A,
    PE11  = 0x4B,
    PE12  = 0x4C,
    PE13  = 0x4D,
    PE14  = 0x4E,
    PE15  = 0x4F,
    //GPIOF
    PF0   = 0x40,
    PF1   = 0x41,
    PF2   = 0x42,
    PF3   = 0x43,
    PF4   = 0x44,
    PF5   = 0x45,
    PF6   = 0x46,
    PF7   = 0x47,
    PF8   = 0x48,
    PF9   = 0x49,
    PF10  = 0x4A,
    PF11  = 0x4B,
    PF12  = 0x4C,
    PF13  = 0x4D,
    PF14  = 0x4E,
    PF15  = 0x4F,
} gpio_pins_t;


/**
  * @brief  Retrives pointer to port's registers from pin.
  * @param  pin   Specifies pin number.
  * @param  GPIOx Pointer to registers of GPIOx port.
  */
static inline GPIO_TypeDef * pin2port(gpio_pins_t pin)
{
  uint8_t port_num = ((uint16_t)(pin & 0xFFF0)) >> 4;
  switch (port_num)
  {
    case 0:
      return GPIOA;
    case 1:
      return GPIOB;
    case 2:
      return GPIOC;
    case 3:
      return GPIOD;
    case 4:
      return GPIOE;
    case 5:
      return GPIOF;
    default:
      break;
  }
  return NULL;
}


/**
  * @brief  Retrives pin number in GPIO register.
  * @param  pin   Specifies pin number.
  * @param  num   Register value with configured pin.
  */
static inline uint32_t pin2pin(gpio_pins_t pin)
{
  return (uint32_t)(1 << (pin & 0x000F));
}


/**
  * @brief  Retrives line number from pin.
  * @param  pin   Specifies pin number.
  * @param  num   Line number.
  */
static inline uint32_t pin2line(gpio_pins_t pin)
{
  return (uint32_t)(pin & 0x000F);
}


/**
  * @brief  Enables port of specified pin.
  * @param  pin   Specifies pin number.
  */
static inline void port_enable(gpio_pins_t pin)
{
  uint8_t port_num = ((uint16_t)(pin & 0xFFF0)) >> 4;
  switch (port_num)
  {
    case 0:
      if (__HAL_RCC_GPIOA_IS_CLK_DISABLED()) __HAL_RCC_GPIOA_CLK_ENABLE();
      break;
    case 1:
      if (__HAL_RCC_GPIOB_IS_CLK_DISABLED()) __HAL_RCC_GPIOB_CLK_ENABLE();
      break;
    case 2:
      if (__HAL_RCC_GPIOC_IS_CLK_DISABLED()) __HAL_RCC_GPIOC_CLK_ENABLE();
      break;
    case 3:
      if (__HAL_RCC_GPIOD_IS_CLK_DISABLED()) __HAL_RCC_GPIOD_CLK_ENABLE();
      break;
    case 4:
      if (__HAL_RCC_GPIOE_IS_CLK_DISABLED()) __HAL_RCC_GPIOE_CLK_ENABLE();
      break;
    case 5:
      if (__HAL_RCC_GPIOF_IS_CLK_DISABLED()) __HAL_RCC_GPIOF_CLK_ENABLE();
      break;
    default:
      break;
  }
}


/**
  * @brief  Disables port of specified pin.
  * @param  pin   Specifies pin number.
  */
static inline void port_disable(gpio_pins_t pin)
{
  uint8_t port_num = ((uint16_t)(pin & 0xFFF0)) >> 4;
  switch (port_num)
  {
    case 0:
      if (__HAL_RCC_GPIOA_IS_CLK_ENABLED()) __HAL_RCC_GPIOA_CLK_DISABLE();
      break;
    case 1:
      if (__HAL_RCC_GPIOB_IS_CLK_ENABLED()) __HAL_RCC_GPIOB_CLK_DISABLE();
      break;
    case 2:
      if (__HAL_RCC_GPIOC_IS_CLK_ENABLED()) __HAL_RCC_GPIOC_CLK_DISABLE();
      break;
    case 3:
      if (__HAL_RCC_GPIOD_IS_CLK_ENABLED()) __HAL_RCC_GPIOD_CLK_DISABLE();
      break;
    case 4:
      if (__HAL_RCC_GPIOE_IS_CLK_ENABLED()) __HAL_RCC_GPIOE_CLK_DISABLE();
      break;
    case 5:
      if (__HAL_RCC_GPIOF_IS_CLK_ENABLED()) __HAL_RCC_GPIOF_CLK_DISABLE();
      break;
    default:
      break;
  }
}


#endif //STM32_GPIO_NAMES_H