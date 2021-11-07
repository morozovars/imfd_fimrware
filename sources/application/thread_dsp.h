#ifndef THREAD_DSP_H_
#define THREAD_DSP_H_


#include "util/err_codes.h"


#define THREAD_DSP_WAKEUP_FLAG        0x00000001


ret_code_t thread_dsp_init(void);


ret_code_t thread_dsp_run(uint8_t ** p_data, uint16_t * p_length);


#endif // THREAD_DSP_H_