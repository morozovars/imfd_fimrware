#ifndef SCOMP_EVENT_SENDER_H_
#define SCOMP_EVENT_SENDER_H_

#include "scomp_err_codes.h"
#include "scomp_typedef.h"
#include "scomp_event_base.h"
//TODO: add discription
scomp_err_code_t scomp_event_sender_init(scomp_buf_cfg_t * p_init);

scomp_err_code_t scomp_events_register(scomp_event_base_t * p_evt);

scomp_err_code_t scomp_send_event(uint8_t event_code, uint8_t* payload_buf, size_t payload_len);

#endif // ! SCOMP_EVENT_SENDER_H_