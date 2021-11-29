#include "fft_sfm.h"
#include <stdbool.h>
#include "arm_const_structs.h"
#include "arm_math.h"
#include "polyfit.h"
#include "gmv_default.h"
#include <math.h>


/**@brief: Macro for calculation of the index of the _freq in FFT specturum.
  *
  */
#define IFR_IND(_freq)    _freq * (FREQ_AFTER_DECIMATION_HZ / TIME_WINDOW_MEAS_COUNT)


/**@brief: Indexes in IFR arrays.
  *
  */
enum
{
    IFR_CURRENT_IDX = 0u,
    IFR_VIB1_IDX,
    IFR_VIB2_IDX,
    IFR_TOTAL_COUNT,
};


/**@brief: Indexes in instant gmv - gmv_buf and gmv_ref arrays.
  *
  */
enum
{
    GMV_IFR1_IDX = 0u,
    GMV_IFR2_IDX = 1u,
    GMV_TOTAL_COUNT,

    GMV_INSTANT_CURRENT_IDX = GMV_IFR1_IDX,
    GMV_INSTANT_IFR1_VIB_IDX = GMV_IFR1_IDX,
    GMV_INSTANT_IFR2_VIB_IDX = GMV_IFR2_IDX,
    GMV_INSTANT_COUNT = GMV_TOTAL_COUNT,

    GMV_REF_CURRENT_IDX = GMV_IFR1_IDX,
    GMV_REF_1_VIB_IDX = GMV_IFR1_IDX,
    GMV_REF_2_VIB_IDX = GMV_IFR2_IDX,
    GMV_REF_TOTAL_COUNT = GMV_TOTAL_COUNT,
};


/**@brief: Indexes in default_ref_gmv.
  *
  */
enum 
{
    DEFAULT_REF_GMV_CURRENT_IDX = 0u,
    DEFAULT_REF_GMV1_VIB1_IDX,
    DEFAULT_REF_GMV2_VIB1_IDX,
    DEFAULT_REF_GMV1_VIB2_IDX,
    DEFAULT_REF_GMV2_VIB2_IDX,
    DEFAULT_REF_GMV_TOTAL_COUNT,

    CALIB_REF_GMV_CURRENT_IDX =       DEFAULT_REF_GMV_CURRENT_IDX,
    CALIB_REF_GMV1_VIB1_IDX =         DEFAULT_REF_GMV1_VIB1_IDX,
    CALIB_REF_GMV2_VIB1_IDX =         DEFAULT_REF_GMV2_VIB1_IDX,
    CALIB_REF_GMV1_VIB2_IDX =         DEFAULT_REF_GMV1_VIB2_IDX,
    CALIB_REF_GMV2_VIB2_IDX =         DEFAULT_REF_GMV2_VIB2_IDX,
    CALIB_REF_GMV_TOTAL_COUNT =       DEFAULT_REF_GMV_TOTAL_COUNT,
};


static uint32_t sample_freq = 1000;
static imfd_meas_t decimated_measurement[TIME_WINDOW_MEAS_COUNT];
static POINT_PRECISION cmplx_fft_result[TIME_WINDOW_MEAS_COUNT][2];
static uint32_t cur_decimation_count = 0;
static uint32_t decimation_coef;
static uint32_t cur_timewindow_count = 0;
static imfd_meas_type_t meas_type = IMFD_MEAS_VIB_RADIAL;
static arm_rfft_fast_instance_f64 m_rfft;
static const POINT_PRECISION * p_ifr[IFR_TOTAL_COUNT] = {
    [IFR_CURRENT_IDX] = decimated_measurement[IFR_IND(IFR1_CUR_FREQ1)].data.raw,
    [IFR_VIB1_IDX] = decimated_measurement[IFR_IND(IFR1_VIB_FREQ1)].data.raw,
    [IFR_VIB2_IDX] = decimated_measurement[IFR_IND(IFR2_VIB_FREQ1)].data.raw
};
static const uint16_t ifr_length[IFR_TOTAL_COUNT] = {
    [IFR_CURRENT_IDX] = IFR_IND(IFR1_CUR_FREQ2) - IFR_IND(IFR1_CUR_FREQ1),
    [IFR_VIB1_IDX] = IFR_IND(IFR1_VIB_FREQ2) - IFR_IND(IFR1_VIB_FREQ1),
    [IFR_VIB2_IDX] = IFR_IND(IFR2_VIB_FREQ2) - IFR_IND(IFR2_VIB_FREQ1),
};
static POINT_PRECISION gmv_buf[GMV_INSTANT_COUNT][GMV_P];
static POINT_PRECISION gmv_ref[GMV_REF_TOTAL_COUNT][GMV_P];
static const POINT_PRECISION default_gmv_ref[DEFAULT_REF_GMV_TOTAL_COUNT][GMV_P] = {
    [DEFAULT_REF_GMV_CURRENT_IDX] = DEFAULT_GMV_CURRENT,
    [DEFAULT_REF_GMV1_VIB1_IDX] = DEFAULT_GMV1_VIB1,
    [DEFAULT_REF_GMV2_VIB1_IDX] = DEFAULT_GMV2_VIB1,
    [DEFAULT_REF_GMV1_VIB2_IDX] = DEFAULT_GMV1_VIB2,
    [DEFAULT_REF_GMV2_VIB2_IDX] = DEFAULT_GMV2_VIB2,
};
static POINT_PRECISION poly_coefs[2];


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


static POINT_PRECISION max_array(POINT_PRECISION * p_src, uint32_t src_length)
{
    POINT_PRECISION max_val = -DBL_MAX;
    ///POINT_PRECISION max_val = -FLT_MAX;
    for (uint32_t i = 0; i < src_length; i++)
    {
        if (max_val < p_src[i])
        {
            max_val = p_src[i];
        }
    }
    return max_val;
}


static void sfm_gmv(POINT_PRECISION * p_src, POINT_PRECISION * p_dst, uint32_t src_length)
{
    POINT_PRECISION max_val = max_array(p_src, src_length);
    POINT_PRECISION moment_order[GMV_P];
    for (uint32_t i = 0; i < src_length; i++)
    {
        cmplx_fft_result[0][i] = p_src[i] / max_val;
    }
    for (uint32_t i = 0; i < GMV_P; i++)
    {
        moment_order[i] = exp(GMV_MIN_ORDER + i * ((GMV_MAX_ORDER - GMV_MIN_ORDER) / GMV_P));
        POINT_PRECISION sum = 0;
        for (uint32_t j = 0; j < src_length; j++)
        {
            sum += pow(cmplx_fft_result[0][j], moment_order[i]) / src_length;
        }
        p_dst[i] = pow(sum, (1/moment_order[i]));
        p_dst[i] = p_dst[i] * max_val;
    }

}


imfd_ret_t fft_sfm_init(void)
{
    /// Reset counters.
    cur_decimation_count = 0;
    cur_timewindow_count = 0;
    decimation_coef = sample_freq / FREQ_AFTER_DECIMATION_HZ;

    /// Init CMSIS's FFT.
    arm_status init_code = arm_rfft_fast_init_f64(&m_rfft, TIME_WINDOW_MEAS_COUNT);
    if (init_code != ARM_MATH_SUCCESS)
    {
        return IMFD_ERROR;
    }

    /// Init parameters.
    meas_type = IMFD_MEAS_VIB_RADIAL;

    /// Set initial reference GMV.
    fft_sfm_set_ref_gmv(IMFD_REF_GMV_LOAD_DEFAULT, NULL);

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

    /// Calculate rFFT.
    arm_rfft_fast_f64(&m_rfft, decimated_measurement[0].data.raw, cmplx_fft_result[0], 0u);
    cmplx_mag(cmplx_fft_result[0], decimated_measurement[0].data.raw, TIME_WINDOW_MEAS_COUNT);

    // caclulate GMVs.
    if (meas_type == IMFD_MEAS_SINGLE_CURRENT)
    {
        sfm_gmv((POINT_PRECISION *)p_ifr[IFR_CURRENT_IDX], gmv_buf[GMV_INSTANT_CURRENT_IDX], ifr_length[IFR_CURRENT_IDX]);
    } else if ((meas_type == IMFD_MEAS_VIB_AXIAL) || (meas_type == IMFD_MEAS_VIB_RADIAL))
    {
        sfm_gmv((POINT_PRECISION *)p_ifr[IFR_VIB1_IDX], gmv_buf[GMV_INSTANT_IFR1_VIB_IDX], ifr_length[IFR_VIB1_IDX]);
        sfm_gmv((POINT_PRECISION *)p_ifr[IFR_VIB2_IDX], gmv_buf[GMV_INSTANT_IFR2_VIB_IDX], ifr_length[IFR_VIB2_IDX]);
    }

    /// Calculate slopes.
    if (meas_type == IMFD_MEAS_SINGLE_CURRENT)
    {
        polyfit(gmv_ref[GMV_REF_CURRENT_IDX], gmv_buf[GMV_INSTANT_CURRENT_IDX], GMV_P, 1, poly_coefs);
    } else if ((meas_type == IMFD_MEAS_VIB_AXIAL) || (meas_type == IMFD_MEAS_VIB_RADIAL))
    {
        polyfit(gmv_ref[GMV_REF_1_VIB_IDX], gmv_buf[GMV_INSTANT_IFR1_VIB_IDX], GMV_P, 1, poly_coefs);
        polyfit(gmv_ref[GMV_REF_2_VIB_IDX], gmv_buf[GMV_INSTANT_IFR2_VIB_IDX], GMV_P, 1, &poly_coefs[1]);
    }

    return IMFD_DRDY;
}

void fft_sfm_set_fs(uint32_t new_freq)
{
    sample_freq = new_freq;
    decimation_coef = sample_freq / FREQ_AFTER_DECIMATION_HZ;
}


void fft_sfm_set_meas_type(imfd_meas_type_t new_type)
{
    meas_type = new_type;
}


void fft_sfm_get_meas_type(imfd_meas_type_t * p_meas_type)
{
    *p_meas_type = meas_type;
}


void fft_sfm_get_result(POINT_PRECISION ** p_slope, uint16_t * p_len)
{
    *p_slope = poly_coefs;
    switch (meas_type)
    {
        case IMFD_MEAS_VIB_AXIAL:
        case IMFD_MEAS_VIB_RADIAL:
            *p_len = IFR_VIB_COUNT * sizeof(POINT_PRECISION);
            break;
        case IMFD_MEAS_SINGLE_CURRENT:
            *p_len = IFR_CUR_COUNT * sizeof(POINT_PRECISION);
            break;
#ifdef USE_VECTOR_PARAMETERS
        case IMFD_MEAS_VIB_DOUBLE:
            break;
        case IMFD_MEAS_THREE_PHASES_CURRENTS:
            break;
#endif
        default:
            break;
    }
}


void fft_sfm_get_fft_buf(POINT_PRECISION ** p_buf, uint16_t * p_len)
{
    /// When fft_sfm_signal_processing return IMFD_DRDY, in buffer decimated_measurement Fourier spectrum.
    switch (meas_type)
    {
        case IMFD_MEAS_VIB_AXIAL:
        case IMFD_MEAS_VIB_RADIAL:
            *p_buf = (POINT_PRECISION *)p_ifr[IFR_VIB2_IDX];
            *p_len = (ifr_length[IFR_VIB2_IDX] * sizeof(POINT_PRECISION));
            break;
        case IMFD_MEAS_SINGLE_CURRENT:
            *p_buf = (POINT_PRECISION *)p_ifr[IFR_CURRENT_IDX];
            *p_len = (ifr_length[IFR_CURRENT_IDX] * sizeof(POINT_PRECISION));
            break;
#ifdef USE_VECTOR_PARAMETERS
        case IMFD_MEAS_VIB_DOUBLE:
            break;
        case IMFD_MEAS_THREE_PHASES_CURRENTS:
            break;
#endif
        default:
            break;
    }
}


void fft_sfm_get_gmv_buf(POINT_PRECISION ** p_buf)
{
    *p_buf = gmv_buf[0];
}


void fft_sfm_set_ref_gmv(imfd_set_gmv_ref_op_type op_type, POINT_PRECISION * p_gmv_ref)
{
    switch (op_type)
    {
        case IMFD_REF_GMV_LOAD_DEFAULT:
            if (meas_type == IMFD_MEAS_SINGLE_CURRENT)
            {
                memcpy(gmv_ref[GMV_REF_CURRENT_IDX], default_gmv_ref[DEFAULT_REF_GMV_CURRENT_IDX], GMV_P * sizeof(POINT_PRECISION));
            } else if (meas_type == IMFD_MEAS_VIB_AXIAL)
            {
                memcpy(gmv_ref[GMV_REF_1_VIB_IDX], default_gmv_ref[DEFAULT_REF_GMV1_VIB1_IDX], GMV_P * sizeof(POINT_PRECISION));
                memcpy(gmv_ref[GMV_REF_2_VIB_IDX], default_gmv_ref[DEFAULT_REF_GMV2_VIB1_IDX], GMV_P * sizeof(POINT_PRECISION));
            } else if (meas_type == IMFD_MEAS_VIB_RADIAL)
            {
                memcpy(gmv_ref[GMV_REF_1_VIB_IDX], default_gmv_ref[DEFAULT_REF_GMV1_VIB2_IDX], GMV_P * sizeof(POINT_PRECISION));
                memcpy(gmv_ref[GMV_REF_2_VIB_IDX], default_gmv_ref[DEFAULT_REF_GMV2_VIB2_IDX], GMV_P * sizeof(POINT_PRECISION));
            }
            break;
        case IMFD_REF_GMV_LOAD_FROM_POINTER:
            if (meas_type == IMFD_MEAS_SINGLE_CURRENT)
            {
                memcpy(gmv_ref[GMV_REF_CURRENT_IDX], p_gmv_ref, GMV_P * sizeof(POINT_PRECISION));
            } else if ((meas_type == IMFD_MEAS_VIB_AXIAL) || (meas_type == IMFD_MEAS_VIB_RADIAL))
            {
                memcpy(gmv_ref[GMV_REF_1_VIB_IDX], p_gmv_ref, GMV_P * sizeof(POINT_PRECISION));
                memcpy(gmv_ref[GMV_REF_2_VIB_IDX], p_gmv_ref + GMV_P, GMV_P * sizeof(POINT_PRECISION));
            }
            break;
        case IMFD_REF_GMV_LOAD_FROM_CURRENT:
            if (meas_type == IMFD_MEAS_SINGLE_CURRENT)
            {
                memcpy(gmv_ref[GMV_REF_CURRENT_IDX], gmv_buf[GMV_INSTANT_CURRENT_IDX], GMV_P * sizeof(POINT_PRECISION));
            } else if ((meas_type == IMFD_MEAS_VIB_AXIAL) || (meas_type == IMFD_MEAS_VIB_RADIAL))
            {
                memcpy(gmv_ref[GMV_REF_1_VIB_IDX], gmv_buf[GMV_INSTANT_IFR1_VIB_IDX], GMV_P * sizeof(POINT_PRECISION));
                memcpy(gmv_ref[GMV_REF_2_VIB_IDX], gmv_buf[GMV_INSTANT_IFR2_VIB_IDX], GMV_P * sizeof(POINT_PRECISION));
            }
            break;
#ifdef USE_VECTOR_PARAMETERS
        case IMFD_MEAS_VIB_DOUBLE:
            break;
        case IMFD_MEAS_THREE_PHASES_CURRENTS:
            break;
#endif
        default:
            break;
    }
}

void fft_sfm_get_ref_gmv(POINT_PRECISION ** p_ref_gmv, uint32_t * p_len)
{
    *p_ref_gmv = gmv_ref[0];
    *p_len = sizeof(gmv_ref);
}