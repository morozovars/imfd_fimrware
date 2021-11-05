#include "template_command.h"

#include <string.h>
#include <scomp_command_base.h>
#include <scomp.h>

/* Private functions prototypes BEGIN */

static void get_handler(const uint8_t* in_payload, uint8_t const in_payload_len
                      , uint8_t* out_payload, uint8_t* out_payload_len);

/* Private functions prototypes END */

/* Macroses BEGIN */

SCOMP_DEFINE_ATTRIBUTE(get, 0x02, get_handler, 0)

SCOMP_DEFINE_COMMAND(tmplt, 0x20, 1
                    , &attr_get)

/* Macroses END */

/* Global variables BEGIN */

static uint32_t some_value = 800; // Test: response data = 800

/* Global variables END */

/* Public function BEGIN */

void template_command_init(void)
{
   scomp_register_command(&cmd_tmplt);
}

/* Public function END */

/* Private functions BEGIN */

void get_handler(const uint8_t* in_payload, uint8_t const in_payload_len
                      , uint8_t* out_payload, uint8_t* out_payload_len)
{
  if ((in_payload == NULL) || (out_payload == NULL) || (out_payload_len == NULL))
  {
    return;
  }
  
  // handle in_payload if required
  //

  memcpy(out_payload, &some_value, sizeof(some_value));
  *out_payload_len = sizeof(some_value);  

  (void)in_payload;
  (void)in_payload_len;
  (void)out_payload;
  (void)out_payload_len;
}

/* Private functions END */
