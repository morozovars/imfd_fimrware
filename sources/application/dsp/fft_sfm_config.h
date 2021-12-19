#ifndef FFT_SFM_CONFIG_H_
#define FFT_SFM_CONFIG_H_

/**@brief FFT_SFM: Precision of the fractional values: double or float. @ref fft_sfm_config
  *
  */
#define IMFD_CONFIG_POINT_PRECISION       double


/**@brief FFT_SFM: Timestamp type. @ref fft_sfm_config
  *
  */
#define IMFD_CONFIG_TIMESTAMP_TYPE        double


/**@brief FFT_SFM: Align for measurement structures. @ref fft_sfm_config
  *
  */
#define IMFD_CONFIG_MEAS_STRUCT_ALIGN


/**@brief FFT_SFM: Final frequency after decimation. @ref fft_sfm_config
  *
  */
#define IMFD_CONFIG_FINAL_FREQ_HZ               1000

/**@brief FFT_SFM: Time-window in milliseconds @ref fft_sfm_config
  *
  */
#define IMFD_CONFIG_TIME_WINDOW_MS              1000

/**@brief FFT_SFM: Configuration of the usage of the simultaneously few parameters @ref fft_sfm_config
  *
  */
#ifdef IMFD_CONFIG_USE_VECTOR_PARAMETERS
#undef IMFD_CONFIG_USE_VECTOR_PARAMETERS
#endif


/**@brief FFT_SFM:   Max moment order for GMV @ref fft_sfm_config
  *
  */
#define IMFD_CONFIG_GMV_MAX_ORDER               (15.0f)

/**@brief FFT_SFM:   Min moment order for GMV @ref fft_sfm_config
  *
  */
#define IMFD_CONFIG_GMV_MIN_ORDER               (-15.0f)


/**@brief FFT_SFM:   GMV resolution @ref fft_sfm_config
  *
  */
#define IMFD_CONFIG_GMV_RESOLUTION              (75u)


#endif // FFT_SFM_CONFIG_H_