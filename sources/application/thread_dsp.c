#include "thread_dsp.h"
#include "app_config.h"
#include <string.h>


osMessageQueueId_t * p_new_data_queue;


ret_code_t thread_dsp_init(thread_dsp_init_t * p_init)
{
    p_new_data_queue = p_init->p_new_data_queue;
    return CODE_SUCCESS;
}


ret_code_t thread_dsp_run(uint8_t ** p_data, uint16_t * p_length)
{
    ret_code_t err_code = CODE_SUCCESS;
    app_queue_rx_msg_t msg;
    
    /// Get meas from queue.
    err_code = os_status2code(osMessageQueueGet(*p_new_data_queue, &msg, 0u, 0u));
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }
    //TODO:remove, echo debug only;
    *p_data = msg.buf;
    *p_length = msg.length;

    /// Parse message.

    /// Apply signal processing algorithms.

    return CODE_NEW_DATA;
}