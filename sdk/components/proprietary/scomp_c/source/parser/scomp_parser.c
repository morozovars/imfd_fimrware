#include "scomp_parser.h"

#include <string.h>

#include "scomp_data_sender.h"
#include "xor8.h"

/* Private typedefs BEGIN */

/* Private typedefs END */

/* Private variables BEGIN */

static scomp_command_base_t* commands[256] = { 0 };
static master_message_head_t master_head;
static scomp_buf_cfg_t * cfg = 0;

/* Private variables END */

/* Private functions prototypes BEGIN */

static void send_response(scomp_operation_result_t reult, uint8_t payload_len);

/* Private functions prototypes END */

/* Public function BEGIN */
scomp_err_code_t scomp_parser_init(scomp_buf_cfg_t * p_init)
{
  if(p_init == 0)
  {
    return SCOMP_WRONG_ARGS;
  }
  if((p_init->p_send_buf == 0) || (p_init->send_size == 0))
  {
    return SCOMP_WRONG_ARGS;
  }
  cfg = p_init;
  return SCOMP_SUCCESS;
}

scomp_err_code_t scomp_parser_register_command(scomp_command_base_t const* command)
{
  if (command == 0)
  {
    return SCOMP_WRONG_ARGS;
  }

  if (command->code == 0)
  {
    return SCOMP_WRONG_ARGS;
  }

  commands[command->code] = (scomp_command_base_t*)command; // const cast

  return SCOMP_SUCCESS;
}

scomp_err_code_t scomp_parser_unregister_command(scomp_command_base_t const* command)
{
  if (command == 0)
  {
    return SCOMP_WRONG_ARGS;
  }

  if (command->code == 0)
  {
    return SCOMP_WRONG_ARGS;
  }

  commands[command->code] = 0;

  return SCOMP_SUCCESS;
}

scomp_err_code_t scomp_parser(const uint8_t * input, uint32_t len)
{
  uint32_t input_offset = 0;
  scomp_command_base_t* cmd = 0;
  scomp_attribute_t* attr = 0;
  uint8_t out_payload_len = 0;

  if (input == 0)
  {
    return SCOMP_WRONG_ARGS;
  }

  if (len == 0)
  {
    return SCOMP_WRONG_ARGS;
  }
  
  if ((*input == SCOMP_MESSAGE_HEADER_MASTER) && (cfg->p_send_buf) && (cfg->send_size != 0))
  {
    if (len >= sizeof(master_message_head_t))
    {
      memcpy(&master_head, input, sizeof(master_message_head_t));
      cmd = commands[master_head.opcode];
      input_offset += sizeof(master_message_head_t);
      if ((cmd == 0) || (cmd->code == 0))
      {
        send_response(SCOMP_OPERATION_RESULT_NOT_IMPEMENTED, 0);
        return SCOMP_SUCCESS;
      }
      uint8_t i;
      for(i = 0; i < cmd->attr_count; ++i)
      {
        if (master_head.attribute == cmd->attributes[i]->code)
        {
          attr = (scomp_attribute_t*)cmd->attributes[i]; // const_cast
          if (attr->handler)
          {
            if ((len - input_offset) == attr->requestPayloadSize + 1)
            {
              input_offset += attr->requestPayloadSize + 1;
              if (xor8(input, input_offset - 1) == input[input_offset - 1])
              {
                uint8_t resp_err;
                resp_err = attr->handler(input + sizeof(master_message_head_t), 
                            (cfg->send_size - sizeof(master_message_head_t) - 1), //one byte XOR
                            cfg->p_send_buf + sizeof(master_message_head_t), 
                            &out_payload_len);
                send_response(resp_err, out_payload_len);
                break;
              }
              else
              {
                send_response(SCOMP_OPERATION_RESULT_CHECKSUM_ERROR, 0);
                break;
              }
            }
            // wrong paylod len
            else
            {
              send_response(SCOMP_OPERATION_RESULT_FORMAT_ERROR, 0);
              break;
            }
          }
          // Handler not implemented
          else
          {
            send_response(SCOMP_OPERATION_RESULT_NOT_IMPEMENTED, 0);
            break;
          }
        }
      }
      // attribute not found
      if(i == cmd->attr_count)
      {
        send_response(SCOMP_OPERATION_RESULT_NOT_IMPEMENTED, 0);
      }
    }
    else
    {
      send_response(SCOMP_OPERATION_RESULT_FORMAT_ERROR, 0);
    }
  }
 
  return SCOMP_SUCCESS;
}

/* Public functions END */

/* Private functions BEGIN */

void send_response(scomp_operation_result_t result, uint8_t payload_len)
{
  uint8_t xor_of_message = 0;
  uint32_t message_len = 0;

  slave_response_head_t slave_head;
  
  slave_head.master_message_head.header = SCOMP_MESSAGE_HEADER_SLAVE;
  slave_head.master_message_head.opcode = master_head.opcode;
  slave_head.master_message_head.attribute = master_head.attribute;
  slave_head.result = result;

  memcpy(cfg->p_send_buf, &slave_head, sizeof(slave_response_head_t));
  message_len = sizeof(slave_response_head_t);
  message_len += payload_len;

  xor_of_message = xor8(cfg->p_send_buf, message_len);
  cfg->p_send_buf[message_len] = xor_of_message;
  message_len += 1;
  
  scomp_data_send(cfg->p_send_buf, message_len);
}

/* Private functions END */