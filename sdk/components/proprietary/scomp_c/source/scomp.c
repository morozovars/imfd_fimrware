#include "scomp.h"

#include <stdbool.h>

#include "scomp_data_sender.h"
#include "parser/scomp_parser.h"
#include "scomp_event_sender.h"

/* Public function BEGIN */

scomp_err_code_t scomp_init(scomp_init_t * p_init)
{
  scomp_err_code_t err_code;
  err_code = scomp_parser_init(p_init->p_parser);
  if(err_code != SCOMP_SUCCESS)
  {
      return err_code;
  }
  return scomp_event_sender_init(p_init->p_evt);
}

scomp_err_code_t scomp_register_command(const scomp_command_base_t* cmd)
{
  return scomp_parser_register_command(cmd);
}

scomp_err_code_t scomp_unregister_command(const scomp_command_base_t* cmd)
{
  return scomp_parser_unregister_command(cmd);
}

scomp_err_code_t scomp_register_data_ready_cb(const dataReadyToSendCallback_t cb)
{
  return scomp_data_sender_register_data_ready_cb(cb);
}

scomp_err_code_t scomp_parse(const uint8_t* data, uint32_t len)
{
  return scomp_parser(data, len);
}

/* Public functions END */