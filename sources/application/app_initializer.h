#ifndef APP_INITIALIZER_H_
#define APP_INITIALIZER_H_


#include "util/err_codes.h"


ret_code_t app_init(void);

ret_code_t app_cpu_clock_config(void);

void app_shutdown(void);


#endif //APP_INITIALIZER_H_