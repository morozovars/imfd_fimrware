#include "thread_dsp.h"
#include "fft_sfm.h"
#include "thread_communication.h"
#include "app_config.h"
#include <string.h>

#include "trace/dbgout.h"


#define APP_PRINTF(...)                                                                             \
                                    dbg_printf("<DSP> ");                                          \
                                    dbg_printf(__VA_ARGS__);                                        \
                                    dbg_printf("\n");


osMessageQueueId_t * p_new_data_queue;
static double sum = 0;
static double mean[2];
static uint32_t cur_count = 0;
static uint32_t total_meas_count = 0;


static void process_measurements(uint8_t * p_buf, uint32_t length)
{
    imfd_ret_t ret_code;

    /// Parse message.
    double * p_cur_meas = (double *)p_buf;

    uint32_t meas_count = length / THREAD_MEAS_SIZE;
    for (uint16_t i = 0; i < meas_count; i++)
    {
        /// Apply signal processing algorithms.
        imfd_meas_t meas;
        memcpy(meas.data.raw, &p_cur_meas[i], THREAD_MEAS_SIZE);
        ret_code = fft_sfm_singal_processing(meas);
        if (ret_code == IMFD_DRDY)
        {
            //TODO: get slopes and transmit;
        }

        sum += p_cur_meas[i];
        cur_count++;
        if (cur_count >= 2000)
        {
            mean[0] = sum / 2000;
            mean[1] = mean[0] - 1.0f;
            thread_communication_transmit((uint8_t *)mean, sizeof(mean));
            APP_PRINTF("Result = %2.2f", mean[0]);
            sum = 0.0f;
            cur_count = 0;
        }
        total_meas_count++;
    }
}


ret_code_t thread_dsp_init(thread_dsp_init_t * p_init)
{
    p_new_data_queue = p_init->p_new_data_queue;

    fft_sfm_init();

    return CODE_SUCCESS;
}


ret_code_t thread_dsp_run(void)
{
    ret_code_t ret_code = CODE_SUCCESS;
    app_queue_rx_msg_t meas;
    uint16_t count_msg = 0;
    static double result;
    
    count_msg = osMessageQueueGetCount(*p_new_data_queue);
    while (count_msg)
    {
        /// Get meas from queue.
        ret_code = os_status2code(osMessageQueueGet(*p_new_data_queue, &meas, 0u, 0u));
        if (ret_code == CODE_RESOURCES)
        {
            return CODE_SUCCESS;
        }
        else if (ret_code != CODE_SUCCESS)
        {
            APP_PRINTF("Queue error");
            return ret_code;
        }
        
        //TODO: check length?
        process_measurements(meas.data.buf, meas.length);
    }

    return ret_code;
}