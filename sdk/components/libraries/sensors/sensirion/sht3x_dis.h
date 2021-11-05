#ifndef SHT3X_DIS_H_
#define SHT3X_DIS_H_


#include <stdint.h>


#define	SHT3X_I2C_ADDR_DEFAULT          (0x44u)		/* SHT3x I2C Address (7bit) = 1000100x */
#define	SHT3X_I2C_ADDR_ALT              (0x45u)		/* SHT3x I2C Address (7bit) = 1000101x */
#define SHT3X_BAUD_MAX			(1000000u)	/* Baud Rate max Fast Mode Plus - 1MHz. */

#define SHT3X_RESET_TIME		(1.5E-3)	/* Reset pulse time. */
#define SHT3X_MEAS_RTPB_L_TIME  	(4.5E-3)	/* Meas. Delay max., Low repeatability Mode */
#define SHT3X_MEAS_RTPB_M_TIME  	(6.5E-3)	/* Meas. Delay max., Medium repeatability Mode */
#define SHT3X_MEAS_RTPB_H_TIME  	(15.5E-3)	/* Meas. Delay max., High repeatability Mode */
#define SHT3X_CMD_DELAY_TIME            (1.0E-3)	/* Min. delay between commands to sensor. */

#define CRC8_SHT3X_POLY                 (0x31u)           /* Sensirion SHTCx Polynom 0x31 = x^5 + x^4 + 1 */
#define SHT3X_MEAS_MAX			(UINT16_MAX)
#define SHT3X_MEAS_2TEMP(d)	        ((d) * (float)(175.0f / (SHT3X_MEAS_MAX)) - (float)45.0f)	/* Datasheet */
#define SHT3X_MEAS_2RHUM(d)	        ((d) * (float)(100.0f / (SHT3X_MEAS_MAX)))


typedef enum
{
    SHT3x_DIS_CMD_SINGLE_RPTB_H           = (uint16_t)0x2C06u, /* Single Shot Data Acquisition Mode, High Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_SINGLE_RPTB_M           = (uint16_t)0x2C0Du, /* Single Shot Data Acquisition Mode, Medium Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_SINGLE_RPTB_L           = (uint16_t)0x2C10u, /* Single Shot Data Acquisition Mode, Low Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_SINGLE_RPTB_H_CSTR      = (uint16_t)0x2400u, /* Single Shot Data Acquisition Mode, High Repeatability, Clock Stretch enabled. */
    SHT3x_DIS_CMD_SINGLE_RPTB_M_CSTR      = (uint16_t)0x240Bu, /* Single Shot Data Acquisition Mode, Medium Repeatability, Clock Stretch enabled. */
    SHT3x_DIS_CMD_SINGLE_RPTB_L_CSTR      = (uint16_t)0x2416u, /* Single Shot Data Acquisition Mode, Low Repeatability, Clock Stretch enabled. */
    SHT3x_DIS_CMD_PERIODIC_0_5_MPS_RPTB_H = (uint16_t)0x2032, /* Periodic Data Acquisition Mode, 0.5 MPS, High Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_0_5_MPS_RPTB_M = (uint16_t)0x2024, /* Periodic Data Acquisition Mode, 0.5 MPS, Medium Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_0_5_MPS_RPTB_L = (uint16_t)0x202F, /* Periodic Data Acquisition Mode, 0.5 MPS, Low Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_1_MPS_RPTB_H   = (uint16_t)0x2130, /* Periodic Data Acquisition Mode, 1 MPS, High Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_1_MPS_RPTB_M   = (uint16_t)0x2126, /* Periodic Data Acquisition Mode, 1 MPS, Medium Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_1_MPS_RPTB_L   = (uint16_t)0x212D, /* Periodic Data Acquisition Mode, 1 MPS, Low Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_2_MPS_RPTB_H   = (uint16_t)0x2236, /* Periodic Data Acquisition Mode, 2 MPS, High Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_2_MPS_RPTB_M   = (uint16_t)0x2220, /* Periodic Data Acquisition Mode, 2 MPS, Medium Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_2_MPS_RPTB_L   = (uint16_t)0x222B, /* Periodic Data Acquisition Mode, 2 MPS, Low Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_4_MPS_RPTB_H   = (uint16_t)0x2334, /* Periodic Data Acquisition Mode, 4 MPS, High Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_4_MPS_RPTB_M   = (uint16_t)0x2322, /* Periodic Data Acquisition Mode, 4 MPS, Medium Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_4_MPS_RPTB_L   = (uint16_t)0x2329, /* Periodic Data Acquisition Mode, 4 MPS, Low Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_10_MPS_RPTB_H  = (uint16_t)0x2737, /* Periodic Data Acquisition Mode, 10 MPS, High Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_10_MPS_RPTB_M  = (uint16_t)0x2721, /* Periodic Data Acquisition Mode, 10 MPS, Medium Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_PERIODIC_10_MPS_RPTB_L  = (uint16_t)0x272A, /* Periodic Data Acquisition Mode, 10 MPS, Low Repeatability, Clock Stretch disabled. */
    SHT3x_DIS_CMD_ART                     = (uint16_t)0x2B32, /* Periodic Measurement with ART feature. */
    SHT3x_DIS_CMD_BREAK                   = (uint16_t)0x3093, /* Abort/Break Measurement, switch from periodic meausrement to single shot. */
    SHT3x_DIS_CMD_RESET                   = (uint16_t)0x30A2, /* Reset the device. */
    SHT3x_DIS_CMD_HEATER_ENABLE           = (uint16_t)0x306D, /* Enable heater feature. */
    SHT3x_DIS_CMD_HEATER_DISABLE          = (uint16_t)0x3066, /* Disable heater feature. */
    SHT3x_DIS_CMD_READ_STATUS             = (uint16_t)0xF32D, /* Read out Status register. */
    SHT3x_DIS_CMD_CLEAR_STATUS            = (uint16_t)0x3041, /* Clear Status register. */
    SHT3x_DIS_CMD_READ_SERIALNBR          = (uint16_t)0x3780, /* Read Serial Number. */

    SHT3x_DIS_CMD_ALERT_READ_HIGH_SET     = 0xE11F, /* Read Alert High limit Set Value (Set value is when alert pin set, clear value is when alert pin reset. */
    SHT3x_DIS_CMD_ALERT_READ_HIGH_CLEAR   = 0xE114, /* Read Alert High limit Clear Value. */
    SHT3x_DIS_CMD_ALERT_READ_LOW_SET      = 0xE102, /* Read Alert Low limit Set Value. */
    SHT3x_DIS_CMD_ALERT_READ_LOW_CLEAR    = 0xE109, /* Read Alert Low limit Clear Value. */
    SHT3x_DIS_CMD_ALERT_WRITE_HIGH_SET    = 0x611D, /* Write Alert High limit Set Value (Set value is when alert pin set, clear value is when alert pin reset. */
    SHT3x_DIS_CMD_ALERT_WRITE_HIGH_CLEAR  = 0x6116, /* Write Alert High limit Clear Value. */
    SHT3x_DIS_CMD_ALERT_WRITE_LOW_SET     = 0x6100, /* Write Alert Low limit Set Value. */
    SHT3x_DIS_CMD_ALERT_WRITE_LOW_CLEAR   = 0x610B, /* Write Alert Low limit Clear Value. */
    SHT3x_DIS_ALERT_HUMIDITY_MASK         = 0xFE00, /* Mask in alert register for humidity. */
    SHT3x_DIS_ALERT_TEMP_MASK             = 0xFF80, /* Mask in Alert register for temperature. */

    SHT3x_DIS_CRC_START = 0xFFu,                     /* CRC Start */
    SHT3x_DIS_CRC_POLY = 0x31u,                      /* CRC Polynom */
    SHT3x_DIS_DATA_LEN = sizeof(uint16_t),		/* Command Buffer, Data Length */
    SHT3x_DIS_MEAS_LEN = SHT3x_DIS_DATA_LEN * 2,		/* Measurement Data inc. CRC */
    SHT3x_DIS_MAX_TRANS_LEN = 6,		/* Maximum transaction length */
    SHT3x_DIS_STATUS_LEN  = sizeof(uint16_t),
    SHT3x_DIS_CRC_LEN = sizeof(uint8_t),
} sht3x_dis_cmd_t;


typedef union 
{
    uint16_t u16;
    struct{
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        uint16_t crc_status : 1; // write data checksum status
        uint16_t cmd_status : 1; // command status
        uint16_t reserve0 : 2; // reserved 
        uint16_t reset_detected : 1; // system reset detected 
        uint16_t reserve1 : 5; // reserved 
        uint16_t temp_alert : 1; // temperature tracking alert 
        uint16_t rh_alert : 1; // humidity tracking alert 
        uint16_t reserve2 : 1; // reserved 
        uint16_t heater_status : 1; // heater status 
        uint16_t reserve3 : 1; // reserved 
        uint16_t alert_pending : 1; // alert pending status
#else
        u16t alert_pending : 1;
        u16t reserve3 : 1;
        u16t heater_status : 1;
        u16t reserve2 : 1;
        u16t rh_alert : 1;
        u16t temp_alert : 1;
        u16t reserve1 : 5;
        u16t reset_detected : 1;
        u16t reserve0 : 2;
        u16t cmd_status : 1;
        u16t crc_status : 1;
#endif
    }bit;
} sht3x_status_t;


static inline uint16_t sht3x_cmd_2buf(uint16_t cmd)
{
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    return (((cmd) << 8) | ((cmd) >> 8));
#else
    return (cmd);
#endif
}



static inline uint8_t crc8_calc_msb(uint8_t poly, uint8_t crc, uint8_t dat)	/* MSB first, normal */
{
	enum { N1 = 8, N3 = 0 + 7 }; crc ^= dat;

	for (uint16_t i = N1; i > 0; i--) { crc = ((crc << 1) ^ ((crc & (1u << N3)) ? poly : 0u)); }

	return crc;
}


static inline uint8_t crc8_sht3x_dis(const uint8_t *data, uint16_t len, uint8_t start)
{
	for (uint16_t i = 0; i < len; i++) { start = crc8_calc_msb(SHT3x_DIS_CRC_POLY, start, data[i]); }

	return start;
}


static inline uint16_t sht3x_f2alert(float humidity, float temperature)
{
    uint16_t temp_u16 = (uint16_t)(((temperature + 45.0f) / 175.0f) * (SHT3X_MEAS_MAX));
    uint16_t rh_u16 = (uint16_t)((humidity * SHT3X_MEAS_MAX) / 100.0f);

    temp_u16 &= SHT3x_DIS_ALERT_TEMP_MASK;
    temp_u16 = temp_u16 >> 7;
    rh_u16 &= SHT3x_DIS_ALERT_HUMIDITY_MASK;

    return  temp_u16 | rh_u16;
}


static inline void sht3x_alert2f(uint16_t reg, float * humidity, float * temperature)
{
    uint16_t temp_u16 = (uint16_t)(reg << 7);
    uint16_t rh_u16 = (uint16_t)(reg & SHT3x_DIS_ALERT_HUMIDITY_MASK);
    
    *humidity = SHT3X_MEAS_2RHUM(rh_u16);
    *temperature = SHT3X_MEAS_2TEMP(temp_u16);
}


#endif // SHT3X_DIS_H_