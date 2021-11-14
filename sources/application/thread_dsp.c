#include "thread_dsp.h"
#include "app_config.h"
#include <string.h>
#include "trace/dbgout.h"


osMessageQueueId_t * p_new_data_queue;


ret_code_t thread_dsp_init(thread_dsp_init_t * p_init)
{
    p_new_data_queue = p_init->p_new_data_queue;
    return CODE_SUCCESS;
}

double mean = 0;
uint16_t count;

ret_code_t thread_dsp_run(uint8_t ** p_data, uint16_t * p_length)
{
    ret_code_t err_code = CODE_SUCCESS;
    app_queue_usb_meas_msg_t meas;
    uint16_t count_msg = 0;
    
    count_msg = osMessageQueueGetCount(*p_new_data_queue);
    while (count_msg)
    {
        /// Get meas from queue.
        err_code = os_status2code(osMessageQueueGet(*p_new_data_queue, &meas, 0u, 0u));
        if (err_code == CODE_RESOURCES)
        {
            return CODE_SUCCESS;
        }
        else if (err_code != CODE_SUCCESS)
        {
            return err_code;
        }
        //TODO:remove, echo debug only;
        
        mean += meas.data;
        count++;
        if (count == 50u)
        {
            double old_mean = mean / 50u;
            dbg_printf("mean = %2.2f\n", old_mean);
            *p_data = (uint8_t *)&old_mean;
            *p_length = sizeof(mean);
            mean = 0.0f;
            count = 0u;
            return CODE_NEW_DATA;
        }
        count_msg--;
    }
    /// Parse message.

    /// Apply signal processing algorithms.

    return CODE_SUCCESS;
}