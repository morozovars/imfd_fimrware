#include "thread_dsp.h"
#include "app_config.h"
#include "fft_sfm.h"
#include "thread_communication.h"
#include <string.h>

#include "trace/dbgout.h"


#define APP_PRINTF(...)                                                                            \
    dbg_printf("<DSP> ");                                                                          \
    dbg_printf(__VA_ARGS__);                                                                       \
    dbg_printf("\n");


osMessageQueueId_t * p_new_data_queue;
static uint32_t total_meas_count = 0;


static void process_measurements(uint8_t * p_buf, uint32_t length)
{
    imfd_ret_t ret_code;

    /// Parse message.
    double * p_cur_meas = (double *)p_buf;
    POINT_PRECISION * p_slopes;
    //POINT_PRECISION * p_data;
    uint16_t data_length;

    uint32_t meas_count = length / THREAD_MEAS_SIZE;
    for (uint16_t i = 0; i < meas_count; i++)
    {
        /// Apply signal processing algorithms.
        imfd_meas_t meas;
        memcpy(meas.data.raw, &p_cur_meas[i], THREAD_MEAS_SIZE);
        ret_code = fft_sfm_singal_processing(meas);
        if (ret_code == IMFD_DRDY)
        {
            // TODO: get slopes and transmit;
            //fft_sfm_get_fft_buf(&p_data, &data_length);
            //thread_communication_transmit(
            //    COMMUNICATION_RET_MSG_TYPE_DEBUG_CMPLX_MAG, (uint8_t *)p_data, data_length); // just a part of the spectrum
            //fft_sfm_get_gmv_buf(&p_data);
            //thread_communication_transmit(
            //    COMMUNICATION_RET_MSG_TYPE_DEBUG_GMV, (uint8_t *)p_data, GMV_P * sizeof(POINT_PRECISION));
            fft_sfm_get_result(&p_slopes, &data_length);
            communication_ret_msg_type_t ret = (data_length == sizeof(POINT_PRECISION)) ? COMMUNICATION_RET_MSG_TYPE_SINGLE_SLOPE : COMMUNICATION_RET_MSG_TYPE_DOUBLE_SLOPE;
            thread_communication_transmit(ret, (uint8_t *)p_slopes, data_length);
        }
        total_meas_count++;
    }
}


ret_code_t thread_dsp_init(thread_dsp_init_t * p_init)
{
    p_new_data_queue = p_init->p_new_data_queue;

    imfd_init_t fft_sfm_cfg = {
        .addr_calb_ref_gmv_current = APP_FLASH_ADDR_CALIB_REG_GMV_CURRENT,
        .addr_calb_ref_gmv1_vib1 = APP_FLASH_ADDR_CALIB_REG_GMV_VIB1,
        .addr_calb_ref_gmv2_vib1 = APP_FLASH_ADDR_CALIB_REG_GMV_VIB1 + 1600,
        .addr_calb_ref_gmv1_vib2 = APP_FLASH_ADDR_CALIB_REG_GMV_VIB2,
        .addr_calb_ref_gmv2_vib2 = APP_FLASH_ADDR_CALIB_REG_GMV_VIB2 + 1600,
    };
    fft_sfm_init(&fft_sfm_cfg);

    return CODE_SUCCESS;
}


ret_code_t thread_dsp_run(void)
{
    ret_code_t ret_code = CODE_SUCCESS;
    app_queue_rx_msg_t meas;
    uint16_t count_msg = 0;

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

        // TODO: check length?
        process_measurements(meas.data.buf, meas.length);
    }

    return ret_code;
}

void thread_dsp_stop_meas_stream_cb(void)
{
    APP_PRINTF("Total stream meas count = %d", total_meas_count);
    total_meas_count = 0;
}