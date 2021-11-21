#include "fft_sfm.h"
#include <stdbool.h>


#define TIME_WINDOW_MEAS_COUNT_X      ((TIME_WINDOW_MS * FREQ_AFTER_DECIMATION_HZ) / 1000)
#define TIME_WINDOW_MEAS_COUNT      (1024)


static uint32_t sample_freq = 1000;
static imfd_meas_t decimated_measurement[TIME_WINDOW_MEAS_COUNT];  //TODO
static uint32_t cur_decimation_count = 0;
static uint32_t decimation_coef;
static uint32_t cur_timewindow_count = 0;
static imfd_meas_type_t meas_type = IMFD_MEAS_VIB_RADIAL;
//static arm_cfft_instance_f32 cfft_inst;


static imfd_ret_t decimation(imfd_meas_t meas)
{
    /// Averaging with collecting.
    switch (meas.meas_type)
    {
        case IMFD_MEAS_VIB_RADIAL:
        case IMFD_MEAS_VIB_AXIAL:
        {
            POINT_PRECISION * p_val = &(decimated_measurement[cur_timewindow_count].data.vib_single.vibration);
            POINT_PRECISION * p_new_val = &(meas.data.vib_single.vibration);
            *p_val = (*p_val * cur_decimation_count + *p_new_val) / (cur_decimation_count + 1);
            break;
        }
        case IMFD_MEAS_SINGLE_CURRENT:
        {
            POINT_PRECISION * p_val = &(decimated_measurement[cur_timewindow_count].data.cur_single.current);
            POINT_PRECISION * p_new_val = &(meas.data.cur_single.current);
            *p_val = (*p_val * cur_decimation_count + *p_new_val) / (cur_decimation_count + 1);
            break;
        }
        case IMFD_MEAS_THREE_PHASES_CURRENTS:
        {
            imfd_meas_currents_t * p_val = &(decimated_measurement[cur_timewindow_count].data.cur_all);
            imfd_meas_currents_t * p_new_val = &(meas.data.cur_all);
            p_val->i_u = (p_val->i_u * cur_decimation_count + p_new_val->i_u) / (cur_decimation_count + 1);
            p_val->i_v = (p_val->i_v * cur_decimation_count + p_new_val->i_v) / (cur_decimation_count + 1);
            p_val->i_w = (p_val->i_w * cur_decimation_count + p_new_val->i_w) / (cur_decimation_count + 1);
            break;
        }
        default:
            break;
    }

    cur_decimation_count++;
    if (cur_decimation_count >= (decimation_coef))
    {
        cur_decimation_count = 0;
        return IMFD_DRDY;
    }
    return IMFD_OK;
}


static bool is_time_window_buffer_ready(void)
{
    if (cur_timewindow_count >= TIME_WINDOW_MEAS_COUNT)
    {
        return true;
    }
    return false;
}


imfd_ret_t fft_sfm_init(void)
{
    /// Reset counters.
    cur_decimation_count = 0;
    cur_timewindow_count = 0;
    decimation_coef = sample_freq / FREQ_AFTER_DECIMATION_HZ;

    /// Init CMSIS's FFT.
    //arm_status status = arm_cfft_init_f32(&cfft_inst, TIME_WINDOW_MEAS_COUNT);

    return IMFD_OK;
}


imfd_ret_t fft_sfm_singal_processing(imfd_meas_t meas)
{
    imfd_ret_t code;

    /// Determine meas type.
    meas.meas_type = meas_type;
    
    /// Decimation.
    code = decimation(meas);
    if (code != IMFD_DRDY)
    {
        return code; /// return if code == IMFD_OK or IMFD_ERROR.
    }

    /// Collect time-window.
    cur_timewindow_count++;
    if (is_time_window_buffer_ready() != true)
    {
        return IMFD_OK;
    }
    cur_timewindow_count = 0; /// Reset counter.

    // calculate cFFT
    /// prepare buffer for cFFT
    //arm_cfft_f32(&cfft_inst, fft_buf, 0, 1);
    //arm_cmplx_mag_f32(fft_buf, )

    // select IFR

    // caclulate GMVs

    // calculate slopes
    (void)meas;
    return IMFD_OK;
}


imfd_ret_t fft_sfm_get_result(POINT_PRECISION * p_slope)
{
    (void)p_slope;
    return IMFD_OK;
}

imfd_ret_t fft_sfm_set_fs(uint32_t new_freq)
{
    sample_freq = new_freq;
    decimation_coef = sample_freq / FREQ_AFTER_DECIMATION_HZ;
    return IMFD_OK;
}


imfd_ret_t fft_sfm_set_meas_type(imfd_meas_type_t new_type)
{
    meas_type = new_type;
    return IMFD_OK;
}