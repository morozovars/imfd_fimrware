#include "dma/dma.h"

#include "stm32g4xx_hal.h"

static const DMA_Channel_TypeDef* channels_[DMA_INST_CNT][DMA_CHANNEL_CNT] = 
{
  { DMA1_Channel1
  , DMA1_Channel2
  , DMA1_Channel3
  , DMA1_Channel4
  , DMA1_Channel5
  , DMA1_Channel6
  , DMA1_Channel7 },
  
  { DMA2_Channel1
  , DMA2_Channel2
  , DMA2_Channel3
  , DMA2_Channel4
  , DMA2_Channel5
  , 0
  , 0 },
};

static const uint32_t dma_irqn_[DMA_INST_CNT][DMA_CHANNEL_CNT] = 
{
  { DMA1_Channel1_IRQn
  , DMA1_Channel2_IRQn
  , DMA1_Channel3_IRQn
  , DMA1_Channel4_IRQn
  , DMA1_Channel5_IRQn
  , DMA1_Channel6_IRQn
  , DMA1_Channel7_IRQn },
  
  { DMA2_Channel1_IRQn
  , DMA2_Channel2_IRQn
  , DMA2_Channel3_IRQn
  , DMA2_Channel4_IRQn
  , 0
  , 0
  , 0 },
};


DMA_HandleTypeDef hdma_arr[DMA_INST_CNT][DMA_CHANNEL_CNT];

static void clk_enable(const dma_inst_t inst);
static void clk_disable(const dma_inst_t inst);

typedef void (*halCallbackFuncTypedef)(DMA_HandleTypeDef*);

dma_ret_code_t dma_init(const dma_inst_t inst, const dma_channel_t ch, const dma_cfg_t* cfg)
{
  hdma_arr[inst][ch].Init.Mode;
  hdma_arr[inst][ch].Init.Direction = cfg->direction;

  hdma_arr[inst][ch].Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_arr[inst][ch].Init.MemInc = cfg->mem_incr;

  hdma_arr[inst][ch].Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_arr[inst][ch].Init.PeriphInc = cfg->periph_incr;

  hdma_arr[inst][ch].Init.Priority = DMA_PRIORITY_MEDIUM;

  hdma_arr[inst][ch].Instance = channels_[inst][ch];

  clk_enable(inst);

  HAL_NVIC_SetPriority(dma_irqn_[inst][ch], 8, 0);
  HAL_NVIC_EnableIRQ(dma_irqn_[inst][ch]);

  return hal2code(HAL_DMA_Init(&hdma_arr[inst][ch]));
}

dma_ret_code_t dma_deinit(const dma_inst_t inst, const dma_channel_t ch)
{
  dma_ret_code_t ret_code;

  ret_code = hal2code(HAL_DMA_DeInit(&hdma_arr[inst][ch]));

  clk_disable(inst);

  return CODE_SUCCESS;
}

dma_ret_code_t dma_register_callback(const dma_inst_t inst, const dma_channel_t ch
                                , const dma_callback_id_t cb_id, dma_cb_t cb)
{
  return hal2code(HAL_DMA_RegisterCallback(&hdma_arr[inst][ch], cb_id, (halCallbackFuncTypedef)cb));
}

void* dma_get_channel_for_link(const dma_inst_t inst, const dma_channel_t ch)
{
  return &hdma_arr[inst][ch];
}

void clk_enable(const dma_inst_t inst)
{
  switch(inst)
  {
    case DMA_INST_1:
    {
      if (__HAL_RCC_DMA1_IS_CLK_DISABLED())
      {
        __HAL_RCC_DMA1_CLK_ENABLE();
      }
    }
    break;

    case DMA_INST_2:
    {
      if (__HAL_RCC_DMA2_IS_CLK_DISABLED())
      {
        __HAL_RCC_DMA2_CLK_ENABLE();
      }
    }
    break;

    default:
    break;
  }
}

void clk_disable(const dma_inst_t inst)
{
  switch(inst)
  {
    case DMA_INST_1:
    {
      if (__HAL_RCC_DMA1_IS_CLK_ENABLED())
      {
        __HAL_RCC_DMA1_CLK_DISABLE();
      }
    }
    break;

    case DMA_INST_2:
    {
      if (__HAL_RCC_DMA2_IS_CLK_ENABLED())
      {
        __HAL_RCC_DMA2_CLK_DISABLE();
      }
    }
    break;

    default:
    break;
  }
}

void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_arr[DMA_INST_1][DMA_CHANNEL_1]);
}

void DMA1_Channel2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_arr[DMA_INST_1][DMA_CHANNEL_2]);
}

void DMA1_Channel3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_arr[DMA_INST_1][DMA_CHANNEL_3]);
}

void DMA1_Channel4_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_arr[DMA_INST_1][DMA_CHANNEL_4]);
}

void DMA1_Channel5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_arr[DMA_INST_1][DMA_CHANNEL_5]);
}

void DMA1_Channel6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_arr[DMA_INST_1][DMA_CHANNEL_6]);
}

void DMA1_Channel7_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_arr[DMA_INST_1][DMA_CHANNEL_7]);
}