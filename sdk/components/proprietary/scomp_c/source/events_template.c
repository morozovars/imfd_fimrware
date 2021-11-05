#include "events.h"

#include "scomp_typedef.h"
#include "scomp_event_base.h"
#include "scomp_event_sender.h"

typedef enum
{
  EVENT_ATTR_IM_ALIVE_CODE = 0x01,

  EVENT_ATTR_COUNT = 1
}event_attr_codes_t;

typedef enum
{
  EVENT_ATTR_IM_ALIVE_PAYLOAD_SIZE = 0,
}event_attr_payload_size_t;

static uint8_t payload_buf[128];

/* Private functions prototypes BEGIN */

static void im_alive_handler(const uint8_t* in_payload, uint8_t const in_payload_len
                      , uint8_t* out_payload, uint8_t* out_payload_len);

/* Private functions prototypes END */

SCOMP_DEFINE_ATTRIBUTE(im_avlie, EVENT_ATTR_IM_ALIVE_CODE, im_alive_handler, EVENT_ATTR_IM_ALIVE_PAYLOAD_SIZE)

SCOMP_DEFINE_EVENT(main, SCOMP_OPCODE_EVENT, EVENT_ATTR_COUNT
                    , &attr_im_avlie)


void events_init(void)
{
  return;
}

void im_alive_handler(const uint8_t* in_payload, uint8_t const in_payload_len
                      , uint8_t* out_payload, uint8_t* out_payload_len)
{
  if ((in_payload == NULL) || (out_payload == NULL) || (out_payload_len == NULL))
  {
    return;
  }
  
  *out_payload = 0;
  *out_payload_len = 0;

  (void)in_payload;
  (void)in_payload_len;
  (void)out_payload;
  (void)out_payload_len;
}

void event_send(event_t evt)
{
  uint8_t* payload = 0;
  uint8_t payload_len = 0;
  
  if (evt_main.attributes[evt]->handler)
  {
    evt_main.attributes[evt]->handler(0, 0, payload, payload_len);
  }

  scomp_send_event(evt_main.code, evt_main.attributes[evt]->code, payload, payload_len);
}