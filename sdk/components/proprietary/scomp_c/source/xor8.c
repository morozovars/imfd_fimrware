#include "xor8.h"

uint8_t xor8(const uint8_t* data, uint32_t len)
{
  if (data == 0)
  {
    return 0;
  }

  uint8_t result = 0;
  
  for(uint32_t i = 0; i < len; ++i)
  {
    result ^= data[i];
  }
  
  return result;
}