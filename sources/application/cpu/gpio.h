#ifndef GPIO_H_
#define GPIO_H_


#include "stm32g4xx_hal.h"
#include "stm32_gpio_names.h"


/**
  * @brief   GPIO: Pull-Up or Pull-Down activation.
  */
typedef enum
{
  GPIO_PULL_NO    = GPIO_NOPULL,
  GPIO_PULL_UP    = GPIO_PULLUP,
  GPIO_PULL_DONW  = GPIO_PULLDOWN,
} gpio_pull_t;


/**
  * @brief   GPIO: Frequency (speed) configuration.
  */
typedef enum
{
  GPIO_FREQ_LOW         = GPIO_SPEED_FREQ_LOW,        /*!< range up to 5 MHz, please refer to the product datasheet */
  GPIO_FREQ_MEDIUM      = GPIO_SPEED_FREQ_MEDIUM,     /*!< range  5 MHz to 25 MHz, please refer to the product datasheet */
  GPIO_FREQ_HIGH        = GPIO_SPEED_FREQ_HIGH,       /*!< range 25 MHz to 50 MHz, please refer to the product datasheet */
  GPIO_FREQ_VERY_HIGH   = GPIO_SPEED_FREQ_VERY_HIGH,  /*!< range 50 MHz to 120 MHz, please refer to the product datasheet */
} gpio_freq_t;


/*
  * @brief GPIO: Output Modes.
  */
typedef enum
{
  GPIO_OUTPUT_PUSH_PULL   = GPIO_MODE_OUTPUT_PP,
  GPIO_OUTPUT_OPEN_DRAIN  = GPIO_MODE_OUTPUT_OD,
} gpio_out_t;


/*
  * @brief GPIO: Input modes.
  */
typedef enum
{
  GPIO_IN_DEFAULT               = GPIO_MODE_INPUT,
  GPIO_IN_IT_RAISING            = GPIO_MODE_IT_RISING,
  GPIO_IN_IT_FALLING            = GPIO_MODE_IT_FALLING,
  GPIO_IN_IT_RISING_FALLING     = GPIO_MODE_IT_RISING_FALLING,
  GPIO_IN_EVT_RISING            = GPIO_MODE_EVT_RISING,             /* NOTE: Events not tested. */
  GPIO_IN_EVT_FALLING           = GPIO_MODE_EVT_FALLING,            /* NOTE: Events not tested. */
  GPIO_IN_EVT_RISING_FALLING    = GPIO_MODE_EVT_RISING_FALLING,     /* NOTE: Events not tested. */
} gpio_in_t;


/*
  * @brief GPIO: Alternate modes.
  */
typedef enum
{
    GPIO_AF_PP  = GPIO_MODE_AF_PP,
    GPIO_AF_OD  = GPIO_MODE_AF_OD,
} gpio_af_t;


/*
  * @brief GPIO: Output confuguration structure.
  */
typedef struct
{
  gpio_pull_t     pull;  
  gpio_freq_t     frequency;
  gpio_out_t      type;
} gpio_cfg_out_t; 


/*
  * @brief GPIO: Input confuguration structure.
  */
typedef struct
{
  gpio_pull_t     pull;  
  gpio_freq_t     frequency;
  gpio_in_t       type;
  uint32_t        prio;
} gpio_cfg_in_t; 


/*
  * @brief GPIO: Alternate function confuguration structure.
  */
typedef struct
{
  gpio_af_t       type;
  gpio_pull_t     pull;
  uint32_t        assign_io_mux;
} gpio_cfg_af_t; 


/*
  * @brief GPIO: States.
  */
typedef enum
{
  GPIO_RESET = 0x00,
  GPIO_SET,
}gpio_state_t;


/*
  * @brief GPIO: Callback type.
  */
typedef void (*gpio_cb_t)(void);


/**
  * @brief  Function for configuring the given GPIO pin number as output, hiding inner details.
  * @param  pin   Specifies pin number. Possible pin numbers listed in gpio_pins_t -> stm32_gpio_names.h
  * @param  cfg   Pointer to structure with configuration of the pin
  */
void gpio_pin_cfg_output(gpio_pins_t pin, gpio_cfg_out_t * cfg);


/**
  * @brief  Function for configuring the given GPIO pin number as input, hiding inner details.
  * @param  pin   Specifies pin number. Possible pin numbers listed in gpio_pins_t -> stm32_gpio_names.h
  * @param  cfg   Pointer to structure with configuration of the pin
  */
void gpio_pin_cfg_input(gpio_pins_t pin, gpio_cfg_in_t * cfg);


//TODO: comments;
void gpio_pin_cfg_af(gpio_pins_t pin, gpio_cfg_af_t * cfg);


/**
  * @brief  Register callback for a dedicated GPIO pin.
  * @param  pin   Specifies pin number. Possible pin numbers listed in gpio_pins_t -> stm32_gpio_names.h
  * @param  p_cb  Function pointer to be stored as callback.
  */
void gpio_pin_cfg_register_cb(gpio_pins_t pin, gpio_cb_t * p_cb);


/**
  * @brief  Set high level on the specified GPIO pin.
  * @param  pin   Specifies pin number. Possible pin numbers listed in gpio_pins_t -> stm32_gpio_names.h
  * @retval The input port value
  */
void gpio_pin_set(gpio_pins_t pin);


/**
  * @brief  Set low level on the specified GPIO pin.
  * @param  pin   Specifies pin number. Possible pin numbers listed in gpio_pins_t -> stm32_gpio_names.h
  * @retval The input port value
  */
void gpio_pin_clean(gpio_pins_t pin);


/**
  * @brief  Initialize the required by application GPIOs.
  * @param  GPIOx where x can be (A..G) to select the GPIO peripheral for STM32G4xx family
  * @param  GPIO_Pin specifies the port bit to be written.
  *         This parameter can be any combination of GPIO_PIN_x where x can be (0..15).
  * @retval None
  */
void gpio_pin_toggle(gpio_pins_t pin);


/**
  * @brief  Read the specified GPIO pin.
  * @param  pin   Specifies pin number. Possible pin numbers listed in gpio_pins_t -> stm32_gpio_names.h
  * @retval The input port value
  */
gpio_state_t gpio_pin_read(gpio_pins_t pin);


/**
  * @brief  Remove configurations for the specified pin.
  * @param  pin   Specifies pin number. Possible pin numbers listed in gpio_pins_t -> stm32_gpio_names.h
  */
void gpio_pin_remove_cfg(gpio_pins_t pin);//TODO: not properly tested


#endif //GPIO_H_