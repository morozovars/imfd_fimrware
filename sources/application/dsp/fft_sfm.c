#include "fft_sfm.h"
#include <stdbool.h>
#include "arm_const_structs.h"
#include "arm_math.h"
#include <math.h>

//TODO: selection of the closest to power of 2 value based on calculated.
#define TIME_WINDOW_MEAS_COUNT_X      ((TIME_WINDOW_MS * FREQ_AFTER_DECIMATION_HZ) / 1000)
#define TIME_WINDOW_MEAS_COUNT      (1024)


static uint32_t sample_freq = 1000;
static imfd_meas_t decimated_measurement[TIME_WINDOW_MEAS_COUNT];
static POINT_PRECISION cmplx_fft_result[2][TIME_WINDOW_MEAS_COUNT];
static uint32_t cur_decimation_count = 0;
static uint32_t decimation_coef;
static uint32_t cur_timewindow_count = 0;
static imfd_meas_type_t meas_type = IMFD_MEAS_VIB_RADIAL;
static arm_rfft_fast_instance_f64 m_rfft;
static uint32_t ifr_idx[2][IFR_MAX_COUNT];


static imfd_ret_t decimation(imfd_meas_t meas)
{
    /// Averaging with collecting.
    switch (meas_type)
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
#ifdef USE_VECTOR_PARAMETERS
        case IMFD_MEAS_THREE_PHASES_CURRENTS:
        {
            imfd_meas_currents_t * p_val = &(decimated_measurement[cur_timewindow_count].data.cur_all);
            imfd_meas_currents_t * p_new_val = &(meas.data.cur_all);
            p_val->i_u = (p_val->i_u * cur_decimation_count + p_new_val->i_u) / (cur_decimation_count + 1);
            p_val->i_v = (p_val->i_v * cur_decimation_count + p_new_val->i_v) / (cur_decimation_count + 1);
            p_val->i_w = (p_val->i_w * cur_decimation_count + p_new_val->i_w) / (cur_decimation_count + 1);
            break;
        }
        case IMFD_MEAS_VIB_DOUBLE:
        {
            imfd_meas_currents_t * p_val = &(decimated_measurement[cur_timewindow_count].data.vib_double);
            imfd_meas_currents_t * p_new_val = &(meas.data.vib_double);
            p_val->vibration1 = (p_val->vbiration1 * cur_decimation_count + p_new_val->vibration1) / (cur_decimation_count + 1);
            p_val->vibration2 = (p_val->vibration2 * cur_decimation_count + p_new_val->vibration2) / (cur_decimation_count + 1);
            break;
        }
#endif
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


static void cmplx_mag(POINT_PRECISION * p_src, POINT_PRECISION * p_dst, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        p_dst[i] = sqrt(p_src[(2*i)+0] * p_src[(2*i)+0] + p_src[(2*i)+1] * p_src[(2*i)+1]);
    }
}


imfd_ret_t fft_sfm_init(void)
{
    /// Reset counters.
    cur_decimation_count = 0;
    cur_timewindow_count = 0;
    decimation_coef = sample_freq / FREQ_AFTER_DECIMATION_HZ;

    /// Init CMSIS's FFT.
    arm_status init_code = arm_rfft_fast_init_f64(&m_rfft, 1024);
    if (init_code != ARM_MATH_SUCCESS)
    {
        return IMFD_ERROR;
    }

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

    /// Check time-window.
    cur_timewindow_count++;
    if (is_time_window_buffer_ready() != true)
    {
        return IMFD_OK;
    } /// if ready, go to next operations.
    cur_timewindow_count = 0; /// Reset counter.

    // calculate cFFT
    arm_rfft_fast_f64(&m_rfft, decimated_measurement[0].data.raw, cmplx_fft_result[0], 0u);
    cmplx_mag(cmplx_fft_result[0], decimated_measurement[0].data.raw, TIME_WINDOW_MEAS_COUNT);

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