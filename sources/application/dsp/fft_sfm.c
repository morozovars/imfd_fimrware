#include "fft_sfm.h"
#include "arm_math.h"
#include "arm_const_structs.h"


#define TIME_WINDOW_MEAS_COUNT      ((TIME_WINDOW_MS * FREQ_AFTER_DECIMATION_HZ) / 1000)


static uint32_t sample_freq;
static imfd_meas_t decimated_measurement[TIME_WINDOW_MEAS_COUNT];
static uint32_t cur_decimation_count = 0;
static uint32_t decimation_coef;
static uint32_t cur_timewindow_count = 0;


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
            *p_val = (*p_val * cur_decimation_count + p_new_val) / (cur_decimation_count + 1);
            break;
        }
        case IMFD_MEAS_SINGLE_CURRENT:
        {
            POINT_PRECISION * p_val = &(decimated_measurement[cur_timewindow_count].data.cur_single.current;
            POINT_PRECISION * p_new_val = &(meas.data.cur_single.current);
            *p_val = (*p_val * cur_decimation_count + p_new_val) / (cur_decimation_count + 1);
            break;
        }
        case IMFD_MEAS_THREE_PHASES_CURRENTS:
        {
            POINT_PRECISION * p_val_iu = &(decimated_measurement[cur_timewindow_count].data.cur_all.i_u;
            POINT_PRECISION * p_val_iv = &(decimated_measurement[cur_timewindow_count].data.cur_all.i_v;
            POINT_PRECISION * p_val_iw = &(decimated_measurement[cur_timewindow_count].data.cur_all.i_w;
            POINT_PRECISION * p_new_val_iu = &(meas.data.cur_all.i_u);
            POINT_PRECISION * p_new_val_iv = &(meas.data.cur_all.i_v);
            POINT_PRECISION * p_new_val_iw = &(meas.data.cur_all.i_w);
            *p_val_iu = (*p_val_iu * cur_decimation_count + p_new_val_iu) / (cur_decimation_count + 1);
            *p_val_iv = (*p_val_iv * cur_decimation_count + p_new_val_iv) / (cur_decimation_count + 1);
            *p_val_iw = (*p_val_iw * cur_decimation_count + p_new_val_iw) / (cur_decimation_count + 1);
            break;
        }
        default:
            break;
    }
    decimated_measurement[cur_timewindow_count] = 

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
    cur_decimation_count = 0;
    cur_timewindow_count = 0;
    return IMFD_OK;
}


imfd_ret_t fft_sfm_singal_processing(imfd_meas_t meas)
{
    imfd_ret_t code;
    
    /// Decimation.
    code = decimation(meas);
    if (code != IMFD_DRDY)
    {
        return code; /// return if code == IMFD_OK or IMFD_ERROR.
    }



    /// collect time-window
    cur_timewindow_count++;
    if (is_time_window_buffer_ready() != true)
    {
        return IMFD_OK;
    }
    cur_timewindow_count = 0; /// Reset counter.

    // calculate FFT

    // select IFR

    // caclulate GMVs

    // calculate slopes
    (void)meas;
    return IMFD_OK;
}


imfd_ret_t fft_sfm_get_result(POINT_PRECISION p_slope);

imfd_ret_t fft_sfm_set_fs(uint32_t new_freq);