#ifndef MLX90614_H_
#define MLX90614_H_


#include <stdint.h>

/* I2C (SMBus) settings.  */
#define	MLX90614_I2C_ADDR_DEFAULT           (0x5Au)		      /* MLX90614x I2C Address (7bit) = 1011010x */
#define MLX90614_BAUD_MAX                   (100000u)                 /* Baud Rate max - 100kHz. */

/* Timings. */
#define MLX90614_RESET_TIME		    (20E-3 + 0.25f)           /* Tpor + Tvalid time. */
#define MLX90614_WAKEUP_TIME                (0.33f)                   /* twake. */
#define MLX90614_REQUEST                    (1.5E-3)                  /* tREQ - is the time that the SCL should be forced low in order to switch MLX90614 from PWM mode to I2C mode – at least 1.44ms */
#define MLX90614_MEAS_TIME                  (1E-3)                    /* Not from datasheet! */

/* Converters. */
#define MLX90614_MEAS_2TEMP(d)	            ((d) * (float)(0.02f) - 273.15f)        /* Datasheet p.30. */
#define MLX90614_VDD_COMPENSATION(d, vdd)   ((d) - (vdd - 3.0f) * 0.6f)             /* Datasheet p.37. */


/* Commands. */
enum
{
    MLX90614_EEPROM_REG_TO_MAX            = 0x00,
    MLX90614_EEPROM_REG_TO_MIN            = 0x01,
    MLX90614_EEPROM_REG_PWMCTRL           = 0x02,
    MLX90614_EEPROM_REG_TA_RANGE          = 0x03,
    MLX90614_EEPROM_REG_EMISSIVITY        = 0x04,
    MLX90614_EEPROM_REG_CONFIG_REG1       = 0x05,
    MLX90614_EEPROM_SMBUS_ADDR            = 0x0E,
    MLX90614_EEPROM_REG_ID_NUM1           = 0x1C,
    MLX90614_EEPROM_REG_ID_NUM2           = 0x1D,
    MLX90614_EEPROM_REG_ID_NUM3           = 0x1E,
    MLX90614_EEPROM_REG_ID_NUM4           = 0x1F,
    MLX90614_EEPROM_OP                    = 0x20,     /*  EEPROM Access command mask: 001x xxxx. */

    MLX90614_RAM_RAW_DATA_IR_CH1          = 0x04,
    MLX90614_RAM_RAW_DATA_IR_CH2          = 0x05,
    MLX90614_RAM_DATA_TA                  = 0x06,
    MLX90614_RAM_DATA_TO1                 = 0x07,
    MLX90614_RAM_DATA_TO2                 = 0x08,
    MLX90614_RAM_OP                       = 0x00,     /* RAM Access command mask: 0x000x xxxx. */

    MLX90614_CMD_READ_FLAGS               = 0xF0,     /* Readout status flags. */
    MLX90614_CMD_ENTER_SLEEP_MODE         = 0xFF,     /* Switch device to sleep mode. */

    MLX90614_DATA_ERROR_MASK              = 0x8000,   /* 0x8xxx - flag error. */

    MLX90614_CRC8_START                   = 0x00u,    /* CRC Start */
    MLX90614_CRC8_POLY                    = 0x107u,   /* Melexis MLX90614 Polynom 0x107 = x^8 + x^2 + x^1 + 1. */
    MLX90614_DATA_LEN = sizeof(uint16_t),		/* Command Buffer, Data Length */
    MLX90614_MEAS_LEN = MLX90614_DATA_LEN * 2,		/* Measurement Data inc. CRC */
    MLX90614_MAX_TRANS_LEN = 6,		/* Maximum transaction length */
    MLX90614_STATUS_LEN  = sizeof(uint16_t),
    MLX90614_CRC_LEN = sizeof(uint8_t),
};


/* MLX90614 Status type. */
typedef union 
{
    uint16_t u8;
    struct{
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        uint16_t reserved1 : 4;
        uint16_t por_finished : 1;  // 0 - POR on going, 1 - finished.
        uint16_t eeprom_dead : 1;   // 0 - eeprom ok, 1 - eeprom dead.
        uint16_t reserved2 : 1;
        uint16_t eeprom_busy : 1;   // 0 - eeprom ready, 1 - eeprom busy.
        uint16_t reserved3 : 8;
#else
        uint16_t reserved3 : 8;
        uint16_t eeprom_busy : 1;   // 0 - eeprom ready, 1 - eeprom busy.
        uint16_t reserved2 : 1;
        uint16_t eeprom_dead : 1;   // 0 - eeprom ok, 1 - eeprom dead.
        uint16_t por_finished : 1;  // 0 - POR on going, 1 - finished.
        uint16_t reserved1 : 4;
#endif
    }flags;
} mlx90614_status_t;


static inline uint8_t crc8_calc_smbus(uint16_t poly, uint32_t crc, uint8_t dat)	/* MSB first, normal */
{
    enum
    {
        N1 = 8,
        N2 = 0x8000
    };    

    uint32_t u32_poly = poly << 7;
    
    crc ^= dat;
    crc <<= 8;
        
    for (uint16_t i = N1; i > 0; i--)
    {
        if ((crc & N2) != 0)
        {
            crc ^= u32_poly;
        }

        crc <<= 1;
    }
    
    return (uint8_t)(crc >> 8);
}


static inline uint8_t crc8_mlx(const uint8_t *data, uint16_t len, uint8_t start)
{
    for (uint16_t i = 0; i < len; i++)
    {
        start = crc8_calc_smbus((uint8_t)MLX90614_CRC8_POLY, start, data[i]);
    }
    
    return start;
}


#endif // MLX90614_H_