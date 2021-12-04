/* Includes. */
#include "uart.h"
#include "gpio.h"
#include "stm32g4xx_hal.h"


/* Private variables. */
static UART_HandleTypeDef huart_arr[UART_INST_CNT];
static USART_TypeDef * uart_instances[] = {USART1, USART2, USART3, UART4, UART5};
static uart_cb_t p_cbs[UART_INST_CNT][UART_CB_COUNT] = {NULL};


/* Private functions prototypes. */
static void msp_init_cb(UART_HandleTypeDef * huart);
static void msp_deinit_cb(UART_HandleTypeDef * huart);
static void tx_complete_cb(UART_HandleTypeDef * huart);
static void rx_complete_cb(UART_HandleTypeDef * huart);
static void error_cb(UART_HandleTypeDef * huart);
static void abort_cb(UART_HandleTypeDef * huart);
static void wakeup_cb(UART_HandleTypeDef * huart);
static void rx_fifo_full_cb(UART_HandleTypeDef * huart);
static void tx_fifo_empty_cb(UART_HandleTypeDef * huart);


static const pUART_CallbackTypeDef uart_cb[UART_CB_COUNT] = {
    [UART_MSP_INIT_CB_ID] = msp_init_cb,
    [UART_MSP_DEINIT_CB_ID] = msp_deinit_cb,
    [UART_TX_COMPLETE_CB_ID] = tx_complete_cb,
    [UART_RX_COMPLETE_CB_ID] = rx_complete_cb,
    [UART_ERROR_CB_ID] = error_cb,
    [UART_ABORT_COMPLETE_CB_ID] = abort_cb,
    [UART_WAKEUP_CB_ID] = wakeup_cb,
    [UART_RX_FIFO_FULL_CB_ID] = rx_fifo_full_cb,
    [UART_TX_FIFO_EMPTY_CB_ID] = tx_fifo_empty_cb,
};


static inline IRQn_Type id2irq_type(uint8_t id)
{
    switch (id)
    {
        case UART_INST_1:
            return USART1_IRQn;
        case UART_INST_2:
            return USART2_IRQn;
        case UART_INST_3:
            return USART3_IRQn;
        case UART_INST_4:
            return UART4_IRQn;
        case UART_INST_5:
            return UART5_IRQn;
        default:
            break;
    }
    return HardFault_IRQn;
}


static inline uint8_t handle2id(UART_HandleTypeDef * p_huart)
{
    for (uint8_t i = 0; i < UART_INST_CNT; i++)
    {
        if (p_huart == &huart_arr[i])
        {
            return i;
        }
    }
    return UART_INST_CNT;
}


static inline void clk_enable(uint8_t id)
{
    switch (id)
    {
        case UART_INST_1:
            if (__HAL_RCC_USART1_IS_CLK_DISABLED())
                __HAL_RCC_USART1_CLK_ENABLE();
            break;
        case UART_INST_2:
            if (__HAL_RCC_USART2_IS_CLK_DISABLED())
                __HAL_RCC_USART2_CLK_ENABLE();
            break;
        case UART_INST_3:
            if (__HAL_RCC_USART3_IS_CLK_DISABLED())
                __HAL_RCC_USART3_CLK_ENABLE();
            break;
        case UART_INST_4:
            if (__HAL_RCC_UART4_IS_CLK_DISABLED())
                __HAL_RCC_UART4_CLK_ENABLE();
            break;
        case UART_INST_5:
            if (__HAL_RCC_UART5_IS_CLK_DISABLED())
                __HAL_RCC_UART5_CLK_ENABLE();
            break;
    }
}


static inline void clk_disable(uint8_t id)
{
    switch (id)
    {
        case UART_INST_1:
            if (__HAL_RCC_USART1_IS_CLK_ENABLED())
                __HAL_RCC_USART1_CLK_DISABLE();
            break;
        case UART_INST_2:
            if (__HAL_RCC_USART2_IS_CLK_ENABLED())
                __HAL_RCC_USART2_CLK_DISABLE();
            break;
        case UART_INST_3:
            if (__HAL_RCC_USART3_IS_CLK_ENABLED())
                __HAL_RCC_USART3_CLK_DISABLE();
            break;
        case UART_INST_4:
            if (__HAL_RCC_UART4_IS_CLK_ENABLED())
                __HAL_RCC_UART4_CLK_DISABLE();
            break;
        case UART_INST_5:
            if (__HAL_RCC_UART5_IS_CLK_ENABLED())
                __HAL_RCC_UART5_CLK_DISABLE();
            break;
    }
}


static uart_error_t hal_uart_error2uart_error(uint32_t hal_error)
{
    uart_error_t error = UART_ERROR_NONE;
    if (hal_error & HAL_UART_ERROR_PE)
    {
        error |= UART_ERROR_PARITY;
    }
    if (hal_error & HAL_UART_ERROR_NE)
    {
        error |= UART_ERROR_NOISE;
    }
    if (hal_error & HAL_UART_ERROR_FE)
    {
        error |= UART_ERROR_FRAME;
    }
    if (hal_error & HAL_UART_ERROR_ORE)
    {
        error |= UART_ERROR_OVERRUN;
    }
    if (hal_error & HAL_UART_ERROR_DMA)
    {
        error |= UART_ERROR_DMA;
    }
    if (hal_error & HAL_UART_ERROR_RTO)
    {
        error |= UART_ERROR_RECEIVER_TIMEOUT;
    }

    return error;
}


/* Public functions. */
ret_code_t uart_init(uint8_t id, uart_config_t * cfg)
{
    huart_arr[id].Instance = uart_instances[id];
    huart_arr[id].Init.BaudRate = cfg->baud_rate;
    huart_arr[id].Init.WordLength = cfg->word_lenght;
    huart_arr[id].Init.StopBits = cfg->stop_bits;
    huart_arr[id].Init.Parity = cfg->parity;
    huart_arr[id].Init.Mode = cfg->mode;
    huart_arr[id].Init.HwFlowCtl = cfg->hw_flow_ctrl;
    huart_arr[id].Init.OverSampling = cfg->oversampling;
    huart_arr[id].Init.OneBitSampling = cfg->one_bit_sampling;
    huart_arr[id].Init.ClockPrescaler = cfg->clock_prescaller;
    huart_arr[id].AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    clk_enable(id);

    if (cfg->prio != UART_WITHOUT_IT)
    {
        HAL_NVIC_SetPriority(id2irq_type(id), cfg->prio, 0);
        HAL_NVIC_EnableIRQ(id2irq_type(id));
    }

    return (HAL_UART_Init(&huart_arr[id]) != HAL_OK) ? CODE_ERROR : CODE_SUCCESS;
}


ret_code_t uart_uninit(uint8_t id)
{

    // TODO: disable IRQs
    // TODO: uninit pins.
    return (HAL_UART_DeInit(&huart_arr[id]) != HAL_OK) ? CODE_ERROR : CODE_SUCCESS;
}


ret_code_t uart_register_cb(uint8_t id, uart_cb_id_t cb_id, uart_cb_t p_cb)
{
    HAL_UART_CallbackIDTypeDef  hal_cb_id;
    p_cbs[id][cb_id] = p_cb;

    
    switch(cb_id)
    {
        case UART_MSP_INIT_CB_ID:
            hal_cb_id = HAL_UART_MSPINIT_CB_ID;
            break;
        case UART_MSP_DEINIT_CB_ID:
            hal_cb_id = HAL_UART_MSPDEINIT_CB_ID;
            break;
        case UART_TX_COMPLETE_CB_ID:
            hal_cb_id = HAL_UART_TX_COMPLETE_CB_ID;
            break;
        case UART_RX_COMPLETE_CB_ID:
            hal_cb_id = HAL_UART_RX_COMPLETE_CB_ID;
            break;
        case UART_ERROR_CB_ID:
            hal_cb_id = HAL_UART_ERROR_CB_ID;
            break;
        case UART_ABORT_COMPLETE_CB_ID:
            hal_cb_id = HAL_UART_ABORT_COMPLETE_CB_ID;
            break;
        case UART_WAKEUP_CB_ID:
            hal_cb_id = HAL_UART_WAKEUP_CB_ID;
            break;
        case UART_RX_FIFO_FULL_CB_ID:
            hal_cb_id = HAL_UART_RX_FIFO_FULL_CB_ID;
            break;
        case UART_TX_FIFO_EMPTY_CB_ID:
            hal_cb_id = HAL_UART_TX_FIFO_EMPTY_CB_ID;
            break;
        default:
            break;
    }

    HAL_StatusTypeDef hal_code = HAL_UART_RegisterCallback(
        &huart_arr[id], hal_cb_id, uart_cb[cb_id]);
    return hal2code(hal_code);
}


uint32_t uart_link_to_dma_channel(uint8_t id, uart_dma_slot_t slot, const void * p_link)
{
    switch(slot)
    {
        case UART_DMA_SLOT_RX:
            __HAL_LINKDMA(&huart_arr[id], hdmarx, *(DMA_HandleTypeDef*)p_link);
            return CODE_SUCCESS;
        case UART_DMA_SLOT_TX:
            __HAL_LINKDMA(&huart_arr[id], hdmatx, *(DMA_HandleTypeDef*)p_link);
            return CODE_SUCCESS;
        default:
            return CODE_ERROR;
    }
}


uart_error_t uart_get_error(uint8_t id)
{
    return hal_uart_error2uart_error(HAL_UART_GetError(&huart_arr[id]));
    
}


void uart_clean_error(uint8_t id)
{
    if (__HAL_UART_GET_FLAG(&huart_arr[id], UART_FLAG_PE))
    {
        __HAL_UART_CLEAR_PEFLAG(&huart_arr[id]);
    }
    if (__HAL_UART_GET_FLAG(&huart_arr[id], UART_FLAG_NE))
    {
        __HAL_UART_CLEAR_NEFLAG(&huart_arr[id]);
    }
    if (__HAL_UART_GET_FLAG(&huart_arr[id], UART_FLAG_FE))
    {
        __HAL_UART_CLEAR_FEFLAG(&huart_arr[id]);
    }
    if (__HAL_UART_GET_FLAG(&huart_arr[id], UART_FLAG_ORE))
    {
        __HAL_UART_CLEAR_OREFLAG(&huart_arr[id]);
    }
    if (__HAL_UART_GET_FLAG(&huart_arr[id], UART_FLAG_RTOF))
    {
        __HAL_UART_CLEAR_FLAG(&huart_arr[id], UART_FLAG_RTOF);
    }
}


ret_code_t uart_tx(uint8_t id, uint8_t * p_data, uint16_t size, uint32_t timeout)
{
    HAL_StatusTypeDef hal_code = HAL_UART_Transmit(&huart_arr[id], p_data, size, timeout);
    return hal2code(hal_code);
}


ret_code_t uart_rx(uint8_t id, uint8_t * p_data, uint16_t size, uint32_t timeout)
{
    HAL_StatusTypeDef hal_code = HAL_UART_Receive(&huart_arr[id], p_data, size, timeout);
    return hal2code(hal_code);
}


ret_code_t uart_tx_it(uint8_t id, uint8_t * p_data, uint16_t size)
{
    HAL_StatusTypeDef hal_code = HAL_UART_Transmit_IT(&huart_arr[id], p_data, size);
    return hal2code(hal_code);
}


ret_code_t uart_rx_it(uint8_t id, uint8_t * p_data, uint16_t size)
{
    HAL_StatusTypeDef hal_code = HAL_UART_Receive_IT(&huart_arr[id], p_data, size);
    return hal2code(hal_code);
}


ret_code_t uart_tx_dma(uint8_t id, uint8_t * p_data, uint16_t size)
{
    HAL_StatusTypeDef hal_code = HAL_UART_Transmit_DMA(&huart_arr[id], p_data, size);
    return hal2code(hal_code);
}


ret_code_t uart_rx_dma(uint8_t id, uint8_t * p_data, uint16_t size)
{
    HAL_StatusTypeDef hal_code = HAL_UART_Receive_DMA(&huart_arr[id], p_data, size);
    return hal2code(hal_code);
}


ret_code_t uart_abort(uint8_t id)
{
    HAL_StatusTypeDef hal_code = HAL_UART_Abort(&huart_arr[id]);
    return hal2code(hal_code);
}


ret_code_t uart_abort_it(uint8_t id)
{
    HAL_StatusTypeDef hal_code = HAL_UART_Abort_IT(&huart_arr[id]);
    return hal2code(hal_code);
}

uint32_t uart_get_rx_xfer_count_it(uint8_t id)
{
    return huart_arr[id].RxXferSize - huart_arr[id].RxXferCount;
}

void * uart_get_handle_link(uint8_t id)
{
    return (void*) &huart_arr[id];
}

void uart_enable_receive_timeout(uint8_t id, uint32_t timeout)
{
    HAL_UART_EnableReceiverTimeout(&huart_arr[id]);
    HAL_UART_ReceiverTimeout_Config(&huart_arr[id], timeout);
    SET_BIT(huart_arr[id].Instance->CR1, USART_CR1_RTOIE);
}


static void msp_init_cb(UART_HandleTypeDef * huart)
{
    if (p_cbs[handle2id(huart)][UART_MSP_INIT_CB_ID] != NULL)
    {
        p_cbs[handle2id(huart)][UART_MSP_INIT_CB_ID]();
    }
}


static void msp_deinit_cb(UART_HandleTypeDef * huart)
{
    if (p_cbs[handle2id(huart)][UART_MSP_DEINIT_CB_ID] != NULL)
    {
        p_cbs[handle2id(huart)][UART_MSP_DEINIT_CB_ID]();
    }
}


static void tx_complete_cb(UART_HandleTypeDef * huart)
{
    if (p_cbs[handle2id(huart)][UART_TX_COMPLETE_CB_ID] != NULL)
    {
        p_cbs[handle2id(huart)][UART_TX_COMPLETE_CB_ID]();
    }
}


static void rx_complete_cb(UART_HandleTypeDef * huart)
{
    if (p_cbs[handle2id(huart)][UART_RX_COMPLETE_CB_ID] != NULL)
    {
        p_cbs[handle2id(huart)][UART_RX_COMPLETE_CB_ID]();
    }
}


static void error_cb(UART_HandleTypeDef * huart)
{
    if (p_cbs[handle2id(huart)][UART_ERROR_CB_ID] != NULL)
    {
        p_cbs[handle2id(huart)][UART_ERROR_CB_ID]();
    }
}


static void abort_cb(UART_HandleTypeDef * huart)
{
    if (p_cbs[handle2id(huart)][UART_ABORT_COMPLETE_CB_ID] != NULL)
    {
        p_cbs[handle2id(huart)][UART_ABORT_COMPLETE_CB_ID]();
    }
}


static void wakeup_cb(UART_HandleTypeDef * huart)
{
    if (p_cbs[handle2id(huart)][UART_WAKEUP_CB_ID] != NULL)
    {
        p_cbs[handle2id(huart)][UART_WAKEUP_CB_ID]();
    }
}


static void rx_fifo_full_cb(UART_HandleTypeDef * huart)
{
    if (p_cbs[handle2id(huart)][UART_RX_FIFO_FULL_CB_ID] != NULL)
    {
        p_cbs[handle2id(huart)][UART_RX_FIFO_FULL_CB_ID]();
    }
}


static void tx_fifo_empty_cb(UART_HandleTypeDef * huart)
{
    if (p_cbs[handle2id(huart)][UART_TX_FIFO_EMPTY_CB_ID] != NULL)
    {
        p_cbs[handle2id(huart)][UART_TX_FIFO_EMPTY_CB_ID]();
    }
}

void USART1_IRQHandler(void) { HAL_UART_IRQHandler(&huart_arr[UART_INST_1]); }


void USART2_IRQHandler(void) { HAL_UART_IRQHandler(&huart_arr[UART_INST_2]); }


void USART3_IRQHandler(void) { HAL_UART_IRQHandler(&huart_arr[UART_INST_3]); }


void UART4_IRQHandler(void) { HAL_UART_IRQHandler(&huart_arr[UART_INST_4]); }


void UART5_IRQHandler(void) { HAL_UART_IRQHandler(&huart_arr[UART_INST_5]); }