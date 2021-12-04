#ifndef DMA_H_
#define DMA_H_

#include "dma_err_codes.h"

#if TARGET_CPU_MANUFACTURER == STM
  #include "stm32/dma_stm32.h"
#else
#include "dma_port_template.h"
#endif


/**
 * @brief Common configuration
 *        parameters for DMA channel
 *        initialization
 */
typedef struct
{
  dma_mode_t mode; /*< DMA transfers mode (example: Circular/Normal) */
  dma_direction_t direction; /*< Direction of data flow */

  dma_periph_incr_t periph_incr; /*< Automatic increment of preipheral
                                     address enable/disable */
  dma_mem_incr_t mem_incr; /*< Automatic increment of 
                               memory address enable/disable */
  dma_request_t request;  /*< Specifies the request selected for 
                              the specified channel*/
  dma_malign_t   m_data_align; /*< Determine peripherial data
                                  aligh size*/
  dma_palign_t   p_data_align; /*< Determine memory data
                                  align size */
} dma_cfg_t;


/**
 * @typedef Type of callback function
 * @brief Calls from driver when dma_callback_id_t
 *        event occured
 */
typedef void (*dma_cb_t)(void);

/**
 * @brief Function initialize DMA channel with 
 *        selected parameters in configuration
 *        structure
 *
 * @param inst        Name (Instance) of DMA module
 * @param ch          Number of channel in DMA instance
 * @param cfg         Configuration data structure
 *
 * @return            CODE_SUCCESS if instance was initialized
 */
dma_ret_code_t dma_init(const dma_inst_t inst, const dma_channel_t ch, const dma_cfg_t* cfg);

/**
 * @brief Function deinitialize DMA channel
 *
 * @param inst        Name (Instance) of DMA module
 * @param ch          Number of channel in DMA instance
 *
 * @return            CODE_SUCCESS if instance was deinitialized
 */
dma_ret_code_t dma_deinit(const dma_inst_t inst, const dma_channel_t ch);

/**
 * @brief Register callback function
 *        for some event
 *
 * @param inst        Name (Instance) of DMA module
 * @param ch          Number of channel in DMA instance
 * @param cb_id       Event for callback calling
 * @param cb          Function for calling when event occured
 *
 * @return            CODE_SUCCESS if callback is registered
 */
dma_ret_code_t dma_register_callback(const dma_inst_t inst, const dma_channel_t ch
                                , const dma_callback_id_t cb_id, dma_cb_t cb);

/**
 * @brief Returns channel object for
 *        preipheral linking
 *
 * @note    Use carefully, because used void pointer
 *
 * @return  Casted to (void*) channel object 
 */
void* dma_get_channel_for_link(const dma_inst_t inst, const dma_channel_t ch);


#endif // ! DMA_H_