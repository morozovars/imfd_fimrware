/*
  * @file ina233.h 
  * 
  * @brief This is a library for using the ADC/I2C converterINA233
  *        First written on 03/12/2021 by Sergey Tolmachev
  * @par 
  * COPYRIGHT NOTICE: (c) 2021 MIRCOD LTD. All rights reserved. 
  */ 

#ifndef INA233_H
#define INA233_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>


/*I2C slave address descriptions for A1 pin and A0 pin */
#ifndef INA233_DEFAULT_ADDRESS
#define INA233_DEFAULT_ADDRESS      INA233_ADDRESS_40H /*A1=GND A2=GND */
#endif

#define INA233_ADDRESS_40H          (0x40) /*0b1000000  (A1=GND A2=GND) */
#define INA233_ADDRESS_41H          (0x41) /*0b1000001  (A1=GND A2=VDD) */
#define INA233_ADDRESS_42H          (0x42) /*0b1000010  (A1=GND A2=SDA) */
#define INA233_ADDRESS_43H          (0x43) /*0b1000011  (A1=GND A2=SCL) */
#define INA233_ADDRESS_44H          (0x44) /*0b1000100  (A1=VDD A2=GND) */
#define INA233_ADDRESS_45H          (0x45) /*0b1000101  (A1=VDD A2=VDD) */
#define INA233_ADDRESS_46H          (0x46) /*0b1000110  (A1=VDD A2=SDA) */
#define INA233_ADDRESS_47H          (0x47) /*0b1000111  (A1=VDD A2=SCL) */
#define INA233_ADDRESS_48H          (0x48) /*0b1001000  (A1=SDA A2=GND) */
#define INA233_ADDRESS_49H          (0x49) /*0b1001001  (A1=SDA A2=VDD) */
#define INA233_ADDRESS_4AH          (0x4A) /*0b1001010  (A1=SDA A2=SDA) */
#define INA233_ADDRESS_4BH          (0x4B) /*0b1001011  (A1=SDA A2=SCL) */
#define INA233_ADDRESS_4CH          (0x4C) /*0b1001100  (A1=SCL A2=GND) */
#define INA233_ADDRESS_4DH          (0x4D) /*0b1001101  (A1=SCL A2=VDD) */
#define INA233_ADDRESS_4EH          (0x4E) /*0b1001110  (A1=SCL A2=SDA) */
#define INA233_ADDRESS_4FH          (0x4F) /*0b1001111  (A1=SCL A2=SCL) */


/*PMBus commands*/
        /*R/W/S/C/B Access Types;Read,Write,Send byte, Clear, Block Read, */
        /*NB:Number of Data Bytes, V:Default Value, Description  */

//Clears the status registers and rearms the black box registers for udpating
#define INA233_CLEAR_FAULTS          (0x03) /*S, 0, N/A */

//Restores internal registers to the default values
#define INA233_RESTORE_DEFAULT_ALL   (0x12) /*S, 0, N/A */

//Retrieves the device capability
#define INA233_CAPABILITY            (0x19) /*R, 1, 0xB0 */

//Retrieves or stores the output overcurrent warn limit threshold
#define INA233_IOUT_OC_WARN_LIMIT    (0x4A) /*R/W, 2, 0x7FF8 */

//Retrieves or stores the input overvoltage warn limit threshold
#define INA233_VIN_OV_WARN_LIMIT     (0x57) /*R/W, 2, 0x7FF8 */

//Retrieves or stores the input undervoltage warn limit threshold
#define INA233_VIN_UV_WARN_LIMIT     (0x58) /*R/W, 2, 0x0000 */

//Retrieves or stores the output overpower warn limit threshold
#define INA233_PIN_OP_WARN_LIMIT     (0x6B) /*R/W, 2, 0x7FF8 */

//Retrieves information about the device operating status
#define INA233_STATUS_BYTE           (0x78) /*R, 1, 0x00 */

//Retrieves information about the device operating status
#define INA233_STATUS_WORD           (0x79) /*R, 2, 0x1000 */

//Retrieves information about the output current status
#define INA233_STATUS_IOUT           (0x7B) /*R/W/C, 1, 0x00 */

//Retrieves information about the input status
#define INA233_STATUS_INPUT          (0x7C) /*R/W/C, 1, 0x00*/

//Retrieves information about the communications status
#define INA233_STATUS_CML            (0x7E) /*R/W/C, 1, 0x00 */

//Retrieves information about the manufacturer specific device status
#define INA233_STATUS_MFR_SPECIFIC   (0x80) /*R/W/C, 1, 0x20 */

//Retrieves the energy reading measurement
#define INA233_READ_EIN              (0x86) /*B, 6, 0x00/00/00/00/00/00 */

//Retrieves the measurement for the VBUS voltage
#define INA233_READ_VIN              (0x88) /*R, 2, 0x0000 */

//Retrieves the input current measurement, supports both + and - currents
#define INA233_READ_IN               (0x89) /*R, 2, 0x0000 */

//Mirrors READ_VIN
#define INA233_READ_VOUT             (0x8B) /*R, 2, 0x0000 */

//Mirrors READ_IIN for compatibility
#define INA233_READ_IOUT             (0x8C) /*R, 2, 0x0000 */

//Mirrors READ_PIN for compatibility with possible VBUS connections
#define INA233_READ_POUT             (0x96) /*R, 2, 0x0000 */

//Retrieves the input power measurement
#define INA233_READ_PIN              (0x97) /*R, 2, 0x0000 */

//Retrieves the manufacturer ID in ASCII Characters (TI)
#define INA233_MFR_ID                (0x99) /*B, 2, 0x54/49 */

//Retrieves the device number in ASCII Characters (INA233)
#define INA233_MFR_MODEL             (0x9A) /*B, 6, 0x49/4E/41/32/33/33h */

//Retrieves the device revision letter and number in ASCII (e.g.A0)
#define INA233_MFR_REVISION          (0x9B) /*R, 2, 0x41/30 */

//Configures the ADC averaging modes, conversion times, and opr. modes
#define INA233_MFR_ADC_CONFIG        (0xD0) /*R/W, 2, 0x4127 */

//Retrieves the shunt voltage measurement
#define INA233_MFR_READ_VSHUNT       (0xD1) /*R, 2, 0x0000 */

//Allows masking of device warnings
#define INA233_MFR_ALERT_MASK        (0xD2) /*R/W, 1, 0xF0 */

//Allows the value of the current-sense resistor calibration value to be input. Must be programed at power-up. Default value is set to 1.
 #define INA233_MFR_CALIBRATION       (0xD4) /*R/W, 2, 0x0001 */

//Allows the ALERT pin polarity to be changed
#define INA233_MFR_DEVICE_CONFIG     (0xD5) /*R/W, 1, 0x02 */

//Clears the energy accumulator
#define INA233_CLEAR_EIN             (0xD6) /*S, 0, N/A */

//Returns a unique word for the manufacturer ID
#define INA233_TI_MFR_ID             (0xE0) /*R, 2, ASCII TI, 0x5449 */

//Returns a unique word for the manufacturer model
#define INA233_TI_MFR_MODEL          (0xE1) /*R, 2, ASCII 33 */

//Returns a unique word for the manufacturer revision
#define INA233_TI_MFR_REVISION       (0xE2) /*R, 2, ASCII A0 */
/*Coefficients */

/*   Reading telemetry data and warning thresholds
*   X = (1/m)*(Y*(10^-R)-b)
*   X = Calculated realworld value (Volt/Amps/Watts and so forth)
*   m = the slope coefficient
*   Y = a 2-byte, two's complement integer received from the device
*   b = the offset, which is a 2-byte, two's complement integer
*   R = the exponent, which is a 1-byte, two's complement integer
*   R is only necessary in systems where m is required to be an integer (for example, where m can be stored in a
* register of an integrated circuit) and R must only be large enough to yield the desired accuracy
*
*   Writing telemetry data and warning thresholds
*   Y = (m*X + b)*10^R
*   X = the realworld value (volts, amps, watts, temperature, and so forth)
*   m = the slope coefficient, a 2-byte, two's complement integer
*   Y = a 2-byte, two's complement integer written to the device
*   b = the offset which is a 2-byte, two's complement integer
*   R = the exponent, which is a 1-byte, two's complement integer
*/

/*Shunt voltage telemetry and warning coefficients*/
#define INA233_M_VSHUNT_COEF         (4.0f)
#define INA233_R_VSHUNT_COEF         (5.0f)
#define INA233_B_VSHUNT_COEF         (0.0f)

/*Bus voltage telemetry and warning coefficients*/
#define INA233_M_VBUS_COEF           (8.0f)
#define INA233_R_VBUS_COEF           (2.0f)
#define INA233_B_VBUS_COEF           (0.0f)

/*Current and power telemetry and warning coefficients*/
#define INA233_B_CONS_COEF           (0.0f)
#define INA233_P_CONS_COEF           (0.0f)

/* INA233 Telemetry. */
#define INA233_REG_2TELEMETRY(d, m, b, r)       ( ((d/pow(10, r)) - b) / m )
#define INA233_CURRENT_LSB_2M_CURRENT(_current_lsb)     (round((1/_current_lsb)*1000.0f)/1000)    // Datasheet p.17.
#define INA233_CURRENT_LSB_2M_POWER(_current_lsb)       (round((1/_current_lsb)*25000.0f)/1000)   // Datasheet p.17.

#define INA233_CALC_CURRENT_LSB(_max_cur)         (round((_max_cur/32768.0f)*100000.0f)/100000)// Datasheet p.16. (round lsb to nearest current in mA).
#define INA233_CALIB_2REG(_cur_lsb, _r_shunt)     (uint16_t)(0.00512f / ( _cur_lsb * _r_shunt ) )  // Datasheet p.16.



#endif // INA233_H_