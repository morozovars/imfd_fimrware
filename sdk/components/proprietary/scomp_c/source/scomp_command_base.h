#ifndef SCOMP_COMMAND_BASE_H_
#define SCOMP_COMMAND_BASE_H_

#include <stdint.h>
#include "scomp_typedef.h"

/** @brief Handler type for attribute handling
 *  @note  User must use out_payload_max_len param and not exceed payload buffer
 *  @param in_payload Buffer contain input payload of message
 *  @param out_payload_max_len outputn max paylod length in bytes
 *  @param out_payload Buffer for out data
 *  @param out_payload_len Out data len to be transfer in payload
 *  @return scomp_operation_result_t error code
 */
typedef scomp_operation_result_t (*commandAttrHandler)(const uint8_t* in_payload, uint8_t const out_payload_max_len
                                      , uint8_t* out_payload, uint8_t* out_payload_len);

/** @brief Attribute structure */
typedef struct Scomp_Attribute
{
  const uint8_t code;
  const commandAttrHandler handler;
  const uint32_t requestPayloadSize;
}scomp_attribute_t; 

/** @brief Command base structure */
typedef struct Scomp_Command_Base
{
  const uint8_t code;
  const uint8_t attr_count;
  const scomp_attribute_t* attributes[];
}scomp_command_base_t;

/** @def SCOMP_DEFINE_ATTRIBUTE
 *  @brief Statically define a attribute.
 *
 *  Helper macro to statically define a attribute.
 *
 *  @param _label attribute name.
 *  @param code   code value.
 *  @param handler function for handling attribute call
 *  @param pl_len  payload_len
 */
#define SCOMP_DEFINE_ATTRIBUTE(_label, code, handler, pl_len) \
        static scomp_attribute_t attr_##_label = { code, handler, pl_len };


/** @def SCOMP_DEFINE_COMMAND
 *  @brief Statically define a command.
 *
 *  Helper macro to statically define a command.
 *
 *  @param _label command name.
 *  @param code   code value.
 */
#define SCOMP_DEFINE_COMMAND(_label, code, attribute_count, ...)       \
        static scomp_command_base_t cmd_##_label = { code, attribute_count, { __VA_ARGS__ } };

#endif // ! SCOMP_COMMAND_BASE_H_