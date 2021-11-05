#include "core_it.h"


void HardFault_Handler(void)
{
#ifdef DEBUG
  __asm__("BKPT 0");
#endif
  while (1)
  {
      ;
  }
}
