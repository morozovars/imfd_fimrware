#include "scomp_event_sender.h"

#include <string.h>

#include "xor8.h"
#include "scomp_data_sender.h"

static scomp_buf_cfg_t * cfg = NULL;
static scomp_event_base_t  * events = NULL;

scomp_err_code_t scomp_event_sender_init(scomp_buf_cfg_t * p_init)
{
    if((p_init == 0))
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

scomp_err_code_t scomp_events_register(scomp_event_base_t * p_evt)
{
  if(p_evt == 0)
  {
    return SCOMP_WRONG_ARGS;
  }
  if(p_evt->code == 0)
  {
    return SCOMP_WRONG_ARGS;
  }
  events = p_evt;
  return SCOMP_SUCCESS;
}

scomp_err_code_t scomp_send_event(uint8_t event_code, uint8_t* payload_buf, size_t payload_len)
{
  size_t offset = 0;
  uint8_t xor_ = 0;
  slave_response_head_t head;
  uint8_t out_payload_len = 0;
  uint8_t i;

  if((cfg == NULL) || (events == NULL) || (cfg->p_send_buf == NULL) || (cfg->send_size == 0))
  {
      return SCOMP_SUCCESS; //TODO: change to error
  }
  for(i = 0 ; i < events->attr_count; i++)
  {
      if((events->attributes[i]->code == event_code) && (events->attributes[i]->handler))
      {
          events->attributes[i]->handler(payload_buf, (cfg->send_size - sizeof(slave_response_head_t) - 1), //one byte XOR
                                cfg->p_send_buf + sizeof(slave_response_head_t), &out_payload_len);
          break;
      }
  }
  if(i == events->attr_count)
  {
      return SCOMP_SUCCESS;   //TODO: change to error
  }

  head.master_message_head.header = SCOMP_MESSAGE_HEADER_SLAVE;
  head.master_message_head.opcode = event_code;
  head.master_message_head.attribute = events->attributes[i]->code;
  head.result = 0xFF;

  memcpy(cfg->p_send_buf, &head, sizeof(head));
  offset += sizeof(head);
  offset += out_payload_len;
   
  xor_ = xor8(cfg->p_send_buf, offset);
  cfg->p_send_buf[offset] = xor_;
  offset += sizeof(xor_);

  scomp_data_send(cfg->p_send_buf, offset);
  (void) payload_len;
  return SCOMP_SUCCESS;
}