/**
  ******************************************************************************
  * @file    common_interface.c
  * @author  MCD Application Team
  * @brief   Contains common functions used by different interfaces
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Image license SLA0044,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Assigns the given value to the Main Stack Pointer (MSP).
  * @param  TopOfMainStack  Main Stack Pointer value to set.
  * @retval None.
  */
void Common_SetMsp(uint32_t TopOfMainStack)
{
}

/**
  * @brief  Enable IRQ Interrupts.
  * @retval None.
  */
void Common_EnableIrq(void)
{
}

/**
  * @brief  Disable IRQ Interrupts.
  * @retval None.
  */
void Common_DisableIrq(void)
{
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
