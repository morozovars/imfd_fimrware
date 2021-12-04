#ifndef UART_H_
#define UART_H_


#include <stdint.h>
#include <stdbool.h>
#include "util/err_codes.h"
#include "stm32_gpio_names.h"


/**
  * @brief   UART: Use in configuration parameters, to work without interrupts.
  */
#define UART_WITHOUT_IT   (-1)


/**
  * @brief   UART: Intances.
  */
enum {
    UART_INST_1 = 0x00,
    UART_INST_2,       
    UART_INST_3,
    UART_INST_4,       
    UART_INST_5,       
    UART_INST_CNT
};


/**
  * @brief   UART: configuration structure.
  */
typedef struct
{
  uint32_t    baud_rate;
  uint32_t    word_lenght;
  uint32_t    stop_bits;
  uint32_t    parity;
  uint32_t    mode;
  uint32_t    hw_flow_ctrl;
  uint32_t    oversampling;
  uint32_t    one_bit_sampling;
  uint32_t    clock_prescaller;
  int32_t     prio;
} uart_config_t;


/**
  * @brief   UART: callback ids.
  */
typedef enum
{
  UART_MSP_INIT_CB_ID = 0x00,    /* UART Msp Init Callback ID. */
  UART_MSP_DEINIT_CB_ID,         /* UART Msp Deinit Callback ID. */
  UART_TX_COMPLETE_CB_ID,        /* UART Tx Complete Callback ID. */
  UART_RX_COMPLETE_CB_ID,        /* UART Rx Complete Callback ID. */
  UART_ERROR_CB_ID,              /* UART Error Callback ID. */
  UART_ABORT_COMPLETE_CB_ID,     /* UART Abort Complete Callback ID. */
  UART_WAKEUP_CB_ID,             /* UART Wakeup Callback ID. */
  UART_RX_FIFO_FULL_CB_ID,       /* UART Rx Fifo Full Callback ID. */
  UART_TX_FIFO_EMPTY_CB_ID,      /* UART Tx Fifo Empty Callback ID. */
  UART_CB_COUNT,
} uart_cb_id_t;


/**
  * @brief   UART: Errors.
  */
typedef enum
{
  UART_ERROR_NONE =             0x00,                      /* No error.               0b00000000*/
  UART_ERROR_PARITY =           0x01,                      /* Parity error.           0b00000001 */
  UART_ERROR_NOISE  =           UART_ERROR_PARITY * 2,     /* Noise error.            0b00000010*/
  UART_ERROR_FRAME  =           UART_ERROR_NOISE * 2,      /* Frame error.            0b00000100*/
  UART_ERROR_OVERRUN =          UART_ERROR_FRAME * 2,      /* Overrun error.          0b00001000 */
  UART_ERROR_DMA =              UART_ERROR_OVERRUN * 2,    /* DMA transfer error.     0b00010000*/
  UART_ERROR_RECEIVER_TIMEOUT = UART_ERROR_DMA * 2,        /* Receiver Timeout error. 0b00100000*/
  UART_ERROR_CNT,
} uart_error_t;


/**
  * @brief  UART DMA slots
  */
typedef enum
{
  UART_DMA_SLOT_RX,
  UART_DMA_SLOT_TX,
} uart_dma_slot_t;


/**
  * @brief   UART: callback type.
  */
typedef void (*uart_cb_t)(void);


/**
  * @brief  Initialize uart instance.
  * @param  id    UART instance.
  * @param  cfg   Pointer to structure with configuration of the UART instance.
  * @retval       Error code.
  */
ret_code_t uart_init(uint8_t id, uart_config_t * cfg);


/**
  * @brief  Deinitialize uart instance.
  * @param  id    UART instance.
  * @retval       Error code.
  */
ret_code_t uart_uninit(uint8_t id);


/**
  * @brief  Registrate callback.
  * @param  id    UART instance.
  * @param  cb_id Callback ID.
  * @param  p_cb  Pointer to callback.
  * @retval       Error code.
  */
ret_code_t uart_register_cb(uint8_t id, uart_cb_id_t cb_id, uart_cb_t p_cb);


/**
  * @brief  Get error type of the UART instance.
  * @param  id    UART instance.
  * @retval       UART error code type, ref @uart_error_t.
  */
uart_error_t uart_get_error(uint8_t id);


/**
  * @brief  Clean error flags of the UART instance.
  * @param  id    UART instance.
  */
void uart_clean_error(uint8_t id);


/**
  * @brief  Send an amount of data in blocking mode.
  * @param  id          UART instance.
  * @param  p_data      Pointer to data buffer.
  * @param  size        Amount of data elements to be sent.
  * @param  timeout     Timeout duration.
  * @retval             Error code.
  */
ret_code_t uart_tx(uint8_t id, uint8_t * p_data, uint16_t size, uint32_t timeout);


/**
  * @brief  Receive an amount of data in blocking mode.
  * @param  id          UART instance.
  * @param  p_data      Pointer to data buffer.
  * @param  size        Amount of data elements to be sent.
  * @param  timeout     Timeout duration.
  * @retval             Error code.
  */
ret_code_t uart_rx(uint8_t id, uint8_t * p_data, uint16_t size, uint32_t timeout);


/**
  * @brief  Send an amount of data in interrupt mode.
  * @param  id          UART instance.
  * @param  p_data      Pointer to data buffer.
  * @param  size        Amount of data elements to be sent.
  * @retval             Error code.
  */
ret_code_t uart_tx_it(uint8_t id, uint8_t * p_data, uint16_t size);


/**
  * @brief  Receive an amount of data in interrupt mode.
  * @param  id          UART instance.
  * @param  p_data      Pointer to data buffer.
  * @param  size        Amount of data elements to be sent.
  * @retval             Error code.
  */
ret_code_t uart_rx_it(uint8_t id, uint8_t * p_data, uint16_t size);

/**
  * @brief  Send an amount of data in DMA mode.
  * @param  id          UART instance.
  * @param  p_data      Pointer to data buffer.
  * @param  size        Amount of data elements to be sent.
  * @retval             Error code.
  */
ret_code_t uart_tx_dma(uint8_t id, uint8_t * p_data, uint16_t size);


/**
  * @brief  Receive an amount of data in DMA mode.
  * @param  id          UART instance.
  * @param  p_data      Pointer to data buffer.
  * @param  size        Amount of data elements to be sent.
  * @retval             Error code.
  */
ret_code_t uart_rx_dma(uint8_t id, uint8_t * p_data, uint16_t size);


/**
  * @brief  Abort ongoing transfers (blocking mode).
  * @param  id          UART instance.
  * @retval             Error code.
  */
ret_code_t uart_abort(uint8_t id);


/**
  * @brief  Abort ongoing transfers (interrupt mode).
  * @param  id          UART instance.
  * @retval             Error code.
  */
ret_code_t uart_abort_it(uint8_t id);


/**
  * @brief  Get received byte count (interrupt mode).
  * @param  id          UART instance.
  * @retval             Recieved bytes count.
  */
uint32_t uart_get_rx_xfer_count_it(uint8_t id);


/**
  * @brief  Get UART instance handle.
  * @param  id          UART instance.
  * @retval             Pointer to handle to uart structie.
  */
void * uart_get_handle_link(uint8_t id);


/**
  * @brief        Enables receive timeout interrupt
  *                 which can handled in error cb
  * @param  id    UART instance
  * @timeout      Number of bits of timeout
  */
void uart_enable_receive_timeout(uint8_t id, uint32_t timeout);


/**
  * @brief        Connect DMA channel to slot of instance
  * @param  id    UART instance
  * @param  slot  Slot for DMA handling
  * @param  p_link Pointer to DMA_HandleTypeDef   
  */
uint32_t uart_link_to_dma_channel(uint8_t id, uart_dma_slot_t slot, const void * p_link);


#endif //UART_H_