#ifndef SCOMP_H_
#define SCOMP_H_

/* Default libraries includes BEGIN */

#include <stdint.h>

/* Default libraries includes END */

/* Includes BEGIN */

#include "scomp_err_codes.h"
#include "scomp_typedef.h"
#include "scomp_command_base.h"
#include "scomp_event_base.h"
#include "scomp_event_sender.h"

/* Includes END */

/* Macroses BEGIN */
/**
  *@brief Determen max payloads size before protolol initialize
  */
#define SCOMP_MAX_PAYLOADS_DEF(_name, responce_max, event_max) \
        static uint8_t                  resp_send_buf     [responce_max + sizeof(slave_response_head_t) + 1];\
        static uint8_t                  evt_send_buf      [event_max + sizeof(slave_response_head_t) + 1];  \
        static scomp_buf_cfg_t          parser = {.p_send_buf = resp_send_buf, \
                                                  .send_size = (responce_max + sizeof(slave_response_head_t) + 1) };\
        static scomp_buf_cfg_t          event = { .p_send_buf = evt_send_buf ,  \
                                                  .send_size = (event_max + sizeof(slave_response_head_t) + 1)}; \
        static scomp_init_t             _name = { .p_parser = &parser,  \
                                                  .p_evt  = &event };
/* Macroses END */ 

/* Public functions prototypes BEGIN */

/**
  * @brief      Initialization of protocol
  * @param[in]  Pointer to payload size definfition SCOMP_MAX_RESP_PAYLOAD_DEF
  * @retval     Result of initialization.
  */
scomp_err_code_t scomp_init(scomp_init_t * p_init);

/**
  * @brief      Register command
  * @retval     SCOMP_SUCCESS if command is registered.
  */
scomp_err_code_t scomp_register_command(const scomp_command_base_t* cmd);

/**
  * @brief      Unegister command
  * @retval     SCOMP_SUCCESS if command is unregistered.
  */
scomp_err_code_t scomp_unregister_command(const scomp_command_base_t* cmd);

/**
  * @brief      Register SCOMP response data ready handler
  * @param cb   Pointer to handler function
  * @retval     SCOMP_SUCCESS if callback registered
  */
scomp_err_code_t scomp_register_data_ready_cb(dataReadyToSendCallback_t const cb);

/**
  * @brief      Call parser for input data
  * @param data Data to be parsed
  * @param len  Length of data
  * @retval     Code of operation
  */
scomp_err_code_t scomp_parse(const uint8_t* data, uint32_t len);

/* Public functions prototypes END */

#endif // ! SCOMP_H_