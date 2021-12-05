#ifndef DBGOUT_H_
#define DBGOUT_H_


#include "util/err_codes.h"


ret_code_t dbg_init(void);
void dbg_printf_uart(const char *__format, ...);


#if defined(DEBUG)

#include <stdio.h>

#if DEBUGOUT_2ND_INTERFACE == UART
#define dbg_printf(fmt,args...)     dbg_printf_uart(fmt,##args);          \
                                    printf(fmt,##args)
#else
#define dbg_printf(fmt,args...)	printf(fmt,##args)
#endif // DEBUGOUT_2ND_INTERFACE
#else

#define dbg_printf(fmt,args...)


ret_code_t dbg_init(void)
{
    return CODE_SUCCESS;
}

#endif // defined(DEBUG)


#endif