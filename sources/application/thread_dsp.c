#include "thread_dsp.h"
#include <string.h>


ret_code_t thread_dsp_init(void)
{
    return CODE_SUCCESS;
}


ret_code_t thread_dsp_run(uint8_t ** p_data, uint16_t * p_length)
{
    static const char message[] = "Hello from dsp thread!\n";
    *p_data = (uint8_t *)message;
    *p_length = strlen(message);
    return CODE_NEW_DATA;
}