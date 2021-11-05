#ifndef SCOMP_DATA_SENDER_H_
#define SCOMP_DATA_SENDER_H_

#include "scomp_err_codes.h"
#include "scomp_typedef.h"

scomp_err_code_t scomp_data_sender_register_data_ready_cb(dataReadyToSendCallback_t cb);

void scomp_data_send(const uint8_t* data, size_t len);


#endif // ! SCOMP_DATA_SENDER_H_