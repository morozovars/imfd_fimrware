#ifndef DBGOUT_H_
#define DBGOUT_H_


#if defined(DEBUG)

#include <stdio.h>

#define dbg_printf(fmt,args...)	printf(fmt,##args)

#else

#define dbg_printf(fmt,args...)

#endif



#endif