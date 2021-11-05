#ifndef SCOMP_TYPEDEF_H_
#define SCOMP_TYPEDEF_H_


#include <stdint.h>
#include <stdio.h>
#include "util/err_codes.h"


/**
  * @brief   Protocol operational codes values
  */
typedef enum Scomp_Opcode
{
  SCOMP_OPCODE_VALVE            = 0x01,
  SCOMP_OPCODE_PUMP             = 0x02,
  SCOMP_OPCODE_COMPRESSOR       = 0x03,
  SCOMP_OPCODE_PERILSTATIC      = 0x04,
  SCOMP_OPCODE_ROBO             = 0x05,
  SCOMP_OPCODE_TEMPERATURE      = 0x06,
  SCOMP_OPCODE_PH               = 0x07,
  SCOMP_OPCODE_DO               = 0x08,
  SCOMP_OPCODE_LAC              = 0x09,
  SCOMP_OPCODE_GLU              = 0x0A,
  SCOMP_OPCODE_MFC              = 0x0B,
  SCOMP_OPCODE_PRESSURE         = 0x0C,
  SCOMP_OPCODE_CARBON_DIOXIDE   = 0x0D,
  SCOMP_OPCODE_HEATER           = 0x0E,
  SCOMP_OPCODE_BIOMASS          = 0x0F,
  SCOMP_OPCODE_ECRV             = 0x11,
  SCOMP_OPCODE_OXYGEN           = 0x12,
  SCOMP_OPCODE_PRESSURE_CONTROL = 0x13,
  SCOMP_OPCODE_ALL_DATA         = 0x20,
  SCOMP_OPCODE_WHOAMI           = 0xAA,
  SCOMP_OPCODE_STATUS           = 0x30,
  SCOMP_OPCODE_SOUND            = 0x40,
  SCOMP_OPCODE_LIGHT            = 0x50,
  SCOMP_OPCODE_FAN              = 0x60,
  SCOMP_OPCODE_COOLER           = 0x70,
  SCOMP_OPCODE_PID_CONTROLLER   = 0x80,
  SCOMP_OPCODE_EVENT            = 0xEA,
  SCOMP_OPCODE_ALERT            = 0xAE
}scomp_opcode_t;

/**
  * @brief   Operation result codes
  */
typedef enum Scomp_Operation_Result
{
    SCOMP_OPERATION_RESULT_OK = 0,
    SCOMP_OPERATION_RESULT_FORMAT_ERROR,
    SCOMP_OPERATION_RESULT_CHECKSUM_ERROR,
    SCOMP_OPERATION_RESULT_ARGUMENT_ERROR,
    SCOMP_OPERATION_RESULT_NOT_IMPEMENTED,
    SCOMP_OPERATION_RESULT_VALUE_ERROR,
    SCOMP_OPERATION_RESULT_DEVICE_ERROR,
}scomp_operation_result_t;

/**
  * @brief   Message header values
  */
typedef enum Scomp_Message_Header
{
    SCOMP_MESSAGE_HEADER_MASTER = 0xFA,
    SCOMP_MESSAGE_HEADER_SLAVE = 0xF0,
}scomp_message_header_t;

/**
  * @brief   Head part of master message
  */
typedef struct __attribute__((packed, aligned(1))) Master_Message_Head
{
  uint8_t header;
  uint8_t opcode;
  uint8_t attribute;
}master_message_head_t; 

/**
  * @brief   Head part of slave message
  */
typedef struct __attribute__((packed, aligned(1))) Slave_Response_Head
{
  master_message_head_t master_message_head;
  uint8_t result;
}slave_response_head_t;

/**
  * @brief   Iternal buffer init struct
  */
typedef struct
{
    uint8_t     * p_send_buf;
    uint16_t      send_size;
} scomp_buf_cfg_t;

/**
  * @brief   Scomp init struct
  */
typedef struct
{
    scomp_buf_cfg_t     * p_parser;
    scomp_buf_cfg_t     * p_evt;
} scomp_init_t;

typedef ret_code_t (*dataReadyToSendCallback_t)(const uint8_t* data, size_t len);

#endif // ! SCOMP_TYPEDEF_H_