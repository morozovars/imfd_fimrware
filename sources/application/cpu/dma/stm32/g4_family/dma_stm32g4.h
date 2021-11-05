#ifndef SPI_STM32G4_H_
#define SPI_STM32G4_H_

#ifdef __STM32G474_SUBFAMILY 
  #include "g474_subfamily/dma_stm32g474.h"
#else
  #error "stm32 subfamily is undefined"
#endif

#endif // ! SPI_STM32G4_H_