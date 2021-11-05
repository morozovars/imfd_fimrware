#ifndef DMA_PORT_TEMPLATE_H_
#define DMA_PORT_TEMPLATE_H_

#include <stdint.h>

typedef enum
{
  DMA_INST_1 = 0,
  DMA_INST_2,

  DMA_INST_CNT
} dma_inst_t;

typedef enum
{
  DMA_CHANNEL_1 = 0,
  DMA_CHANNEL_2,
  DMA_CHANNEL_3,
  DMA_CHANNEL_4,
  DMA_CHANNEL_5,

  DMA_CHANNEL_CNT
} dma_channel_t;

typedef enum
{
  DMA_DIRECTION_M2P,
  DMA_DIRECTION_P2M,
  DMA_DIRECTION_M2M
} dma_direction_t;

typedef enum
{
  DMA_MODE_NORMAL,
  DMA_MODE_CIRCULAR
} dma_mode_t;

typedef enum
{
  DMA_PERIPH_INCR_ENABLE,
  DMA_PERIPH_INCR_DISABLE
} dma_periph_incr_t;

typedef enum
{
  DMA_MEM_INCR_ENABLE,
  DMA_MEM_INCR_DISABLE
} dma_mem_incr_t;

#endif ! //DMA_PORT_TEMPLATE_H_