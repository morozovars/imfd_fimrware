#ifndef FFT_SFM_H_
#define FFT_SFM_H_


#include <stdint.h>


#ifndef IMFD_CONFIG_POINT_PRECISION 
#define POINT_PRECISION                   double
#else 
#define POINT_PRECISION                   (IMFD_CONFIG_POINT_PRECISION)
#endif // IMFD_CONFIG_POINT_PRECISION

#ifndef IMFD_CONFIG_TIMESTAMP_TYPE 
#define TIMESTAMP_TYPE                    double
#else 
#define TIMESTAMP_TYPE                    (IMFD_CONFIG_TIMESTAMP_TYPE)
#endif // IMFD_CONFIG_TIMESTAMP_TYPE


#ifndef IMFD_CONFIG_MEAS_STRUCT_ALIGN  
#define MEAS_STRUCT_ALIGN                 
#else
#define MEAS_STRUCT_ALIGN                 (IMFD_CONFIG_MEAS_STRUCT_ALIGN)
#endif // IMFD_CONFIG_MEAS_STRUCT_ALIGN

#ifndef IMFD_CONFIG_FINAL_FREQ_HZ
#define FREQ_AFTER_DECIMATION_HZ          (1000)
#else
#define FREQ_AFTER_DECIMATION_HZ          (IMFD_CONFIG_FINAL_FREQ)
#endif

#ifndef IMFD_CONFIG_TIME_WINDOW_MS
#define TIME_WINDOW_MS                    (500)
#else
#define FREQ_AFTER_DECIMATION_MS          (IMFD_CONFIG_FINAL_FREQ)
#endif

#ifndef IMFD_CONFIG_USE_VECTOR_PARAMETERS
#undef USE_VECTOR_PARAMETERS             
#else
#define USE_VECTOR_PARAMETERS
#endif


//TODO: selection of the closest to power of 2 value based on calculated.
#define TIME_WINDOW_MEAS_COUNT_     ((TIME_WINDOW_MS * FREQ_AFTER_DECIMATION_HZ) / 1000)
#define TIME_WINDOW_MEAS_COUNT      (512)



/**
  * @brief: Type of measured parameter(s).
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
  * @brief: Single vibrations signal structure.
  */
typedef struct
{
    POINT_PRECISION     vibration;
} imfd_meas_vib_single_t;



/**
  * @brief: Structure with duo vibration signals.
  */
typedef struct
{
    POINT_PRECISION                 vibration1;
    POINT_PRECISION                 vibration2;
} imfd_meas_vib_double_t;



/**
  * @brief: Structure with single current measurement data.
  */
typedef struct
{
    POINT_PRECISION                 current;
} imfd_meas_single_current_t;



/**
  * @brief: Structure with currents measurement data.
  */
typedef struct
{
    POINT_PRECISION                 i_u;
    POINT_PRECISION                 i_v;
    POINT_PRECISION                 i_w;
} imfd_meas_currents_t;



/**
  * @brief: Measurement structure.
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


enum
{
    IFR1_CUR_FREQ1 = 30u,
    IFR1_CUR_FREQ2 = 70u,
    IFR_CUR_COUNT = 1u,

    IFR1_VIB_FREQ1 = 0u,
    IFR1_VIB_FREQ2 = 250u,
    IFR2_VIB_FREQ1 = 30u,
    IFR2_VIB_FREQ2 = 70u,
    IFR_VIB_COUNT = 2u,

    IFR_MAX_COUNT = IFR_VIB_COUNT,
};


/**
  * @brief: Return code.
  */
typedef enum
{
    IMFD_ERROR = -1u,
    IMFD_OK =     0u,
    IMFD_DRDY =   1u,
} imfd_ret_t;


imfd_ret_t fft_sfm_init(void);
imfd_ret_t fft_sfm_set_fs(uint32_t new_freq);
imfd_ret_t fft_sfm_set_meas_type(imfd_meas_type_t new_type);
imfd_ret_t fft_sfm_singal_processing(imfd_meas_t meas);
void fft_sfm_get_result(POINT_PRECISION * p_slope);
void fft_sfm_get_fft_buf(POINT_PRECISION ** p_buf);


#endif // FFT_SFM_H_