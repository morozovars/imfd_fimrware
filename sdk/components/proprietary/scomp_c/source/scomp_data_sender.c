#include "scomp_data_sender.h"

static dataReadyToSendCallback_t data_ready_handler = 0;

scomp_err_code_t scomp_data_sender_register_data_ready_cb(dataReadyToSendCallback_t cb)
{
  if (cb == 0)
  {
    return SCOMP_WRONG_ARGS;
  }

  data_ready_handler = cb;
  return SCOMP_SUCCESS;
}

void scomp_data_send(const uint8_t* data, size_t len)
{
  if (data_ready_handler)
  {
    data_ready_handler(data, len);
  }
}