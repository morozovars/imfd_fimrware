#ifndef FFT_SFM_H_
#define FFT_SFM_H_


#ifndef IMFD_CONFIG_POINT_PRECISION 
#define POINT_PRECISION                   (double)
#else 
#define POINT_PRECISION                   (IMFD_CONFIG_POINT_PRECISION)
#endif // IMFD_CONFIG_POINT_PRECISION

#ifndef IMFD_CONFIG_TIMESTAMP_TYPE 
#define TIMESTAMP_TYPE                    (double)
#else 
#define TIMESTAMP_TYPE                    (IMFD_CONFIG_TIMESTAMP_TYPE)
#endif // IMFD_CONFIG_TIMESTAMP_TYPE


#ifndef IMFD_CONFIG_MEAS_STRUCT_ALIGN  
#define MEAS_STRUCT_ALIGN                 
#define IMFD_CONFIG_MEAS_STRUCT_ALIGN  
#define MEAS_STRUCT_ALIGN                 (IMFD_CONFIG_MEAS_STRUCT_ALIGN)
#endif // IMFD_CONFIG_MEAS_STRUCT_ALIGN


/**
  * @brief: Type of measured parameter(s).
  */
typedef enum
{
    IMFD_MEAS_VIB_RADIAL,
    IMFD_MEAS_VIB_AXIAL,
    IMFD_MEAS_THREE_PHASES_CURRENTS,
} imfd_meas_type_t;



/**
  * @brief: Single vibrations signal structure.
  */
typedef enum
{
    POINT_PRECISION     vibration;
    TIMESTAMP_TYPE      timestamp;
} imfd_meas_vib_single_t;



/**
  * @brief: Structure with duo vibration signals.
  */
typedef enum
{
    POINT_PRECISION                 vibration1;
    POINT_PRECISION                 vibration2;
    TIMESTAMP_TYPE                  timestamp;
} imfd_meas_vib_double_t



/**
  * @brief: Structure with current measurement data.
  */
typedef enum
{
    POINT_PRECISION                 i_u;
    POINT_PRECISION                 i_v;
    POINT_PRECISION                 i_w;
    TIMESTAMP_TYPE                  timestamp;
} imfd_meas_current_t;



/**
  * @brief: Measurement structure.
  */
typedef struct MEAS_STRUCT_ALIGN
{
    imfd_meas_type_t                meas_type;
    union {
        imfd_meas_vib_single_t      vib_single;
        imfd_meas_vib_double_t      vib_double;
        imfd_meas_current_t         currents;
    } data;
} imfd_meas_t;


typedef enum
{
    IMFD_RES
} imfd_ret_code_t;

#endif // FFT_SFM_H_