#ifndef SCOMP_PARSER_H_
#define SCOMP_PARSER_H_

/* Includes BEGIN */

#include "scomp_err_codes.h"
#include "scomp_typedef.h"
#include "scomp_command_base.h"

/* Includes END */

/* Public functions prototypes BEGIN */

/**
  * @brief        Register send buffers
  * @param input  init stuct defined by using macros SCOMP_PAYLOADS_DEF
  * @retval       SCOMP_SUCCESS if buffer defined 
  */
scomp_err_code_t scomp_parser_init(scomp_buf_cfg_t * p_init);
/**
  * @brief      Register command for parse.
  * @retval     SCOMP_SUCCESS if command is registered.
  */
scomp_err_code_t scomp_parser_register_command(scomp_command_base_t const* command);

/**
  * @brief      Unregister command for parse
  * @retval     SCOMP_SUCCESS if command is registered.
  */
scomp_err_code_t scomp_parser_unregister_command(scomp_command_base_t const* command);

/**
  * @brief        Parse data.
  * @param input  Data to be parsed.
  * @param len    Length of input data.
  * @retval       Result of parsing.
  */
scomp_err_code_t scomp_parser(const uint8_t* input, uint32_t len);

/* Public functions prototypes END */

#endif // ! SCOMP_PARSER_H_