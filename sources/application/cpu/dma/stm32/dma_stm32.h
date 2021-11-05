#ifndef DMA_STM32_H_
#define DMA_STM32_H_

#ifdef __STM32F1xx_FAMILY 
  #include "f1_family/dma_stm32f1.h"
#endif // ! DMA_STM32F1_H_

#ifdef __STM32G4xx_FAMILY 
  #include "g4_family/dma_stm32g4.h"
#endif // ! DMA_STM32G4_H_

#endif // ! DMA_STM32_H_