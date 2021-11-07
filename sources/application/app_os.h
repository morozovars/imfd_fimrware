#ifndef APP_OS_H_
#define APP_OS_H_


#include "cmsis_os.h"


enum
{
    THREAD_BLINKY = 0x00,
    THREAD_DSP,
    THREAD_COMMUNICATION,
    THREAD_RTOS_TRACE,
    THREADS_CNT,
};


enum 
{
    QUEUE_USB_RX,
    QUEUES_COUNT,
};


enum
{
    MUTEXES_COUNT,
};

enum
{
    SEMAPHORE_COUNT,
};


void app_os_init(void);


#endif //APP_OS_H_