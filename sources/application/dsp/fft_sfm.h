#ifndef FFT_SFM_H_
#define FFT_SFM_H_


#include <stdint.h>
#include "fft_sfm_config.h"


/**@brief FFT_SFM: Precision of the fractional values: double or float. @ref fft_sfm_config
  *
  */
#ifndef IMFD_CONFIG_POINT_PRECISION 
#define POINT_PRECISION                   double
#else 
#define POINT_PRECISION                   IMFD_CONFIG_POINT_PRECISION
#endif // IMFD_CONFIG_POINT_PRECISION


/**@brief FFT_SFM: Timestamp type. @ref fft_sfm_config
  *
  */
#ifndef IMFD_CONFIG_TIMESTAMP_TYPE 
#define TIMESTAMP_TYPE                    double
#else 
#define TIMESTAMP_TYPE                    (IMFD_CONFIG_TIMESTAMP_TYPE)
#endif // IMFD_CONFIG_TIMESTAMP_TYPE


/**@brief FFT_SFM: Align for measurement structures. @ref fft_sfm_config
  *
  */
#ifndef IMFD_CONFIG_MEAS_STRUCT_ALIGN  
#define MEAS_STRUCT_ALIGN                 
#else
#define MEAS_STRUCT_ALIGN                 IMFD_CONFIG_MEAS_STRUCT_ALIGN
#endif // IMFD_CONFIG_MEAS_STRUCT_ALIGN


/**@brief FFT_SFM: Final frequency after decimation. @ref fft_sfm_config
  *
  */
#ifndef IMFD_CONFIG_FINAL_FREQ_HZ
#define FREQ_AFTER_DECIMATION_HZ          (1000)
#else
#define FREQ_AFTER_DECIMATION_HZ          IMFD_CONFIG_FINAL_FREQ_HZ
#endif


/**@brief FFT_SFM: Time-window in milliseconds @ref fft_sfm_config
  *
  */
#ifndef IMFD_CONFIG_TIME_WINDOW_MS
#define TIME_WINDOW_MS                    (1000)
#else
#define TIME_WINDOW_MS                    (IMFD_CONFIG_TIME_WINDOW_MS)
#endif


/**@brief FFT_SFM: Configuration of the usage of the simultaneously few parameters @ref fft_sfm_config
  *
  */
#ifndef IMFD_CONFIG_USE_VECTOR_PARAMETERS
#undef USE_VECTOR_PARAMETERS             
#else
#define USE_VECTOR_PARAMETERS
#endif


/**@brief FFT_SFM:   Max moment order for GMV @ref fft_sfm_config
  *
  */
#ifndef IMFD_CONFIG_GMV_MAX_ORDER
#define GMV_MAX_ORDER                     (15.0f)
#else
#define GMV_MAX_ORDER                     (IMFD_CONFIG_GMV_MAX_ORDER)
#endif

/**@brief FFT_SFM:   Min moment order for GMV @ref fft_sfm_config
  *
  */
#ifndef IMFD_CONFIG_GMV_MIN_ORDER
#define GMV_MIN_ORDER                     (-15.0f)
#else
#define GMV_MIN_ORDER                     (IMFD_CONFIG_GMV_MIN_ORDER)
#endif


/**@brief FFT_SFM:   GMV resolution @ref fft_sfm_config
  *
  */
#ifndef IMFD_CONFIG_GMV_RESOLUTION
#define GMV_P                             (75u)
#else
#define GMV_P                             (IMFD_CONFIG_GMV_RESOLUTION)
#endif


/**@brief FFT_SFM: Calculation of the number of the measurements count in time-window.
  *                It must be power of 2.
  */
#define TIME_WINDOW_MEAS_COUNT_     ((TIME_WINDOW_MS * FREQ_AFTER_DECIMATION_HZ) / 1000)
#if (TIME_WINDOW_MEAS_COUNT_ > (2048 + 1024))
  #define TIME_WINDOW_MEAS_COUNT    (4096)
#elif (TIME_WINDOW_MEAS_COUNT_ > (1024 + 512))
  #define TIME_WINDOW_MEAS_COUNT    (2048)
#elif (TIME_WINDOW_MEAS_COUNT_ > (512 + 256))
  #define TIME_WINDOW_MEAS_COUNT    (1024)
#elif (TIME_WINDOW_MEAS_COUNT_ > (256 + 128))
  #define TIME_WINDOW_MEAS_COUNT    (512)
#else
  #define TIME_WINDOW_MEAS_COUNT    (256)
#endif


/**
  * @brief FFT_SFM: Type of measured parameter(s).
  */
typedef enum
{
    IMFD_MEAS_VIB_RADIAL,
    IMFD_MEAS_VIB_AXIAL,
    IMFD_MEAS_SINGLE_CURRENT,
#ifdef USE_VECTOR_PARAMETERS
    IMFD_MEAS_VIB_DOUBLE,
    IMFD_MEAS_THREE_PHASES_CURRENTS,
#endif
} imfd_meas_type_t;


/**
  * @brief FFT_SFM: Single vibrations signal structure.
  */
typedef struct
{
    POINT_PRECISION     vibration;
} imfd_meas_vib_single_t;



/**
  * @brief FFT_SFM: Structure with duo vibration signals.
  */
typedef struct
{
    POINT_PRECISION                 vibration1;
    POINT_PRECISION                 vibration2;
} imfd_meas_vib_double_t;



/**
  * @brief FFT_SFM: Structure with single current measurement data.
  */
typedef struct
{
    POINT_PRECISION                 current;
} imfd_meas_single_current_t;



/**
  * @brief FFT_SFM: Structure with three-phases currents measurements.
  */
typedef struct
{
    POINT_PRECISION                 i_u;
    POINT_PRECISION                 i_v;
    POINT_PRECISION                 i_w;
} imfd_meas_currents_t;



/**
  * @brief FFT_SFM: Measurement structure.
  */
typedef struct MEAS_STRUCT_ALIGN
{
    union {
        imfd_meas_vib_single_t      vib_single;
        imfd_meas_single_current_t  cur_single;
#ifdef USE_VECTOR_PARAMETERS
        imfd_meas_vib_double_t      vib_double;
        imfd_meas_currents_t        cur_all;
        POINT_PRECISION             raw[3];
#else
        POINT_PRECISION             raw[1];
#endif
    } data;
} imfd_meas_t;


/**
  * @brief FFT_SFM: Information Frequency Ranges (IFRs). @ref fft_sfm_config
  */
enum
{
    IFR1_CUR_FREQ1 = 30u,
    IFR1_CUR_FREQ2 = 70u,
    IFR2_CUR_FREQ1 = 0u,
    IFR2_CUR_FREQ2 = 500u,
    IFR_CUR_COUNT = 2u,

    IFR1_VIB_FREQ1 = 25u,
    IFR1_VIB_FREQ2 = 125u,
    IFR2_VIB_FREQ1 = 125u,
    IFR2_VIB_FREQ2 = 225u,
    IFR_VIB_COUNT = 2u,

    IFR_MAX_COUNT = IFR_VIB_COUNT,
};


/**
  * @brief FFT_SFM: Return codes.
  */
typedef enum
{
    IMFD_ERROR = -1u,
    IMFD_OK =     0u,
    IMFD_DRDY =   1u,
} imfd_ret_t;


/**
  * @brief FFT_SFM: Types of sources from where reference GMVs, can be loaded.
  */
typedef enum
{
    IMFD_REF_GMV_LOAD_DEFAULT,
    IMFD_REF_GMV_LOAD_CALIB,
    IMFD_REF_GMV_LOAD_FROM_CURRENT,
} imfd_gmv_ref_source_t;


typedef struct
{
    uint32_t  addr_calb_ref_gmv1_current;
    uint32_t  addr_calb_ref_gmv2_current;
    uint32_t  addr_calb_ref_gmv1_vib1;
    uint32_t  addr_calb_ref_gmv2_vib1;
    uint32_t  addr_calb_ref_gmv1_vib2;
    uint32_t  addr_calb_ref_gmv2_vib2;
} imfd_init_t;


/**
  * @brief: Initialize FFT SFM algorithm module.
  * @note:  This function must be called before calling of the any other functions of this module.
  * @param: p_init  structure with initialization data.
  * @return:  ret_code:
  *                       IMFD_OK - module successfully initialized.
  *                       IMFD_ERROR - error occured during allocation memory for FFT buffers.
  */
imfd_ret_t fft_sfm_init(imfd_init_t * p_init);


/**
  * @brief: Process new measurement.
  * @params:  meas:       new measurement. User must add new data to field 'meas.raw'.
  * @return:  ret_code:
  *                       IMFD_OK - data successfully treated.
  *                       IMFD_DRDY - data successfully treated, results of treatment are ready
  *                                   and available through fft_sfm_get_result() function.
  */
imfd_ret_t fft_sfm_singal_processing(imfd_meas_t meas);


/**
  * @brief: Get result of the FFT+SFM algorithm.
  * @note   Assumed that this function is called after fft_sfm_singal_processing()
  *         when it returned IMFD_DRDY, otherwise this function returns not actual value.
  * @params:  p_meas_type:  pointer to imfd_meas_type_t where current measurement type
  *                         will be loaded.
  */
void fft_sfm_get_result(POINT_PRECISION ** p_slope, uint16_t * p_len);


/**
  * @brief: Set Fs(sampling frequency).
  * @details: This function results in adjustment of the decimation coefficient.
  * @params:  meas:       new measurement.
  */
void fft_sfm_set_fs(uint32_t new_freq);


/**
  * @brief: Set measurement type.
  * @params:  meas:       new type of measurements.
  */
void fft_sfm_set_meas_type(imfd_meas_type_t new_type);


/**
  * @brief: Get measurement type.
  * @params:  p_meas_type:  pointer to imfd_meas_type_t where current measurement type
  *                         will be loaded.
  */
void fft_sfm_get_meas_type(imfd_meas_type_t * p_meas_type);


/**
  * @brief: Set reference GMV. Action depends on op_type
  * @params:  source:  operation type:
  *                     IMFD_REF_GMV_LOAD_DEFAULT - load default reference GMV (see gmv_default.h).
  *                     IMFD_REG_GMV_LOAD_CALIB - load calib reference GMV.
  *                     IMFD_REG_GMV_LOAD_FROM_CURRENT - current values of the instant GMV will loaded
  *                                                      as reference GMV.
  * @params:  p_gmv_ref:  pointer to reference GMV, which will be used only if op_type == IMFD_REG_GMV_LOAD_FROM_POINTER.
  */
void fft_sfm_set_ref_gmv(imfd_gmv_ref_source_t source);


/**
  * @brief: Get pointer to current reference GMV.
  * @params:  p_ref_gmv:  pointer to pointer, where will be stored pointer on current reference GMV buffer.
  * @params:  p_len:  pointer where length of the reference GMV will be stored.
  */
void fft_sfm_get_ref_gmv(POINT_PRECISION ** p_ref_gmv, uint32_t * p_len);


/**
  * @brief: Get FFT buffer.
  * @purpose: DEBUG ONLY
  * @params:  p_buf:  pointer to pointer on buffer with FFT spectrum.
  * @params:  p_len:  pointer where length of FFT buffer will loaded.
  */
void fft_sfm_get_fft_buf(POINT_PRECISION ** p_buf, uint16_t * p_len);


/**
  * @brief: Get GMV buffer.
  * @purpose: DEBUG ONLY
  * @params:  p_buf:  pointer to pointer on buffer with GMV spectrum.
  */
void fft_sfm_get_gmv_buf(POINT_PRECISION ** p_buf);


/**
  * @brief:   Reset buffer indexes.
  */
void fft_sfm_reset(void);


#endif // FFT_SFM_H_