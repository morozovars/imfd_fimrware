#include "app_os.h"
#include "app_config.h"
#include "app_initializer.h"
#include "board_config.h"
#include "thread_blinky.h"
#include "thread_communication.h"
#include "thread_rtos_trace.h"
#include "trace/dbgout.h"
#include "trace/err_handler.h"


static osThreadId_t thread_ids[THREADS_CNT];
//static osMessageQueueId_t queues_ids[QUEUES_COUNT];
//static osMutexId_t mutexes_ids[MUTEXES_COUNT];
//static osSemaphoreId_t semaphore_ids[SEMAPHORE_COUNT];


static void blinky_thread(void * argument)
{
    thread_blinky_init();
    while (1)
    {
        thread_blinky_run();
        if (err_check())
        {
            break;
        }
    }
    osThreadExit();
    (void)argument;
}


static void dsp_thread(void * argument)
{
    ret_code_t err_code = CODE_SUCCESS;
    //thread_dsp_init();
    while (1)
    {
        //err_code = thread_dsp_run();
        osDelay(1); //TODO:remove
        if (err_code != CODE_SUCCESS)
        {
            err_set(sys_app_thread);
        }
        if (err_check())
        {
            break;
        }
    }
    app_shutdown();
    osThreadExit();
    (void)argument;
}



static void communication_thread(void * argument)
{
    while (1)
    {
        //thread_communication_run();
        osDelay(10);

        if (err_check())
        {
            break;
        }
    }
    osThreadExit();
    (void)argument;
}



static void rtos_trace_thread(void * argument)
{
    static uint32_t tick;

    tick = osKernelGetTickCount();

    while (1)
    {

        tick += THREAD_RTOS_TRACE_PERIOD_MS;
        osDelayUntil(tick);

        thread_rtos_trace_run(thread_ids);
        if (err_check())
        {
            break;
        }
    }
    osThreadExit();
    (void)argument;
}


static void threads_init(void)
{
    osThreadAttr_t attr = {0};

    attr.name = "blinky";
    attr.priority = osPriorityLow;
    attr.stack_size = 512; // Stack size should be therefore 4 byte aligned in order to avoid division caused side effects.
    thread_ids[THREAD_BLINKY] = osThreadNew(blinky_thread, NULL, &attr);

    attr.name = "dsp";
    attr.priority = osPriorityNormal;
    attr.stack_size = 4096;
    thread_ids[THREAD_DSP] = osThreadNew(dsp_thread, NULL, &attr);

    attr.name = "communication";
    attr.priority = osPriorityAboveNormal;
    attr.stack_size = 512;
    thread_ids[THREAD_COMMUNICATION] = osThreadNew(communication_thread, NULL, &attr);


    attr.name = "trace";
    attr.priority = osPriorityHigh;
    attr.stack_size = 1024;
    thread_ids[THREAD_RTOS_TRACE] = osThreadNew(rtos_trace_thread, NULL, &attr);
}


static void queues_init(void)
{
    ;
}


static void mutexes_init(void)
{
    ;
}


static void semaphores_init(void)
{
    ;
}


void app_os_init(void)
{
    queues_init();
    mutexes_init();
    semaphores_init();
    threads_init();

    thread_blinky_init();
    thread_communication_init();

    dbg_printf("<App OS> Initialization successfull\n");
    dbg_printf("<App OS> Application starts.\n");
}