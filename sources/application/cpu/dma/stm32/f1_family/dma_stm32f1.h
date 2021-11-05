#ifndef DMA_STM32_H_
#define DMA_STM32_H_

#ifdef __STM32F103_SUBFAMILY 
  #include "f103_subfamily/dma_stm32f103.h"
#else
  #error "stm32 subfamily is undefined"
#endif

#endif // ! DMA_STM32_H_