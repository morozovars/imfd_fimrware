#include "app_os.h"
#include "app_config.h"
#include "app_initializer.h"
#include "board_config.h"
#include "thread_blinky.h"
#include "thread_dsp.h"
#include "thread_communication.h"
#include "thread_rtos_trace.h"
#include "trace/dbgout.h"
#include "trace/err_handler.h"


static osThreadId_t thread_ids[THREADS_CNT];
static osMessageQueueId_t queues_ids[QUEUES_COUNT];
static uint8_t pool_queue_usb_rx[APP_QUEUE_RX_MSG_COUNT * (APP_QUEUE_RX_MSG_SIZE)];
static uint8_t pool_queue_usb_tx[APP_QUEUE_TX_MSG_COUNT * (APP_QUEUE_TX_MSG_SIZE)];
static uint8_t cb_queue[QUEUES_COUNT][sizeof(StaticQueue_t)];
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
    while (1)
    {
        osThreadFlagsWait(THREAD_DSP_WAKEUP_FLAG, osFlagsWaitAny, osWaitForever);

        err_code = thread_dsp_run();
        if (err_code != CODE_SUCCESS)
        {
            err_set(sys_app_thread);
        }
        if (err_check())
        {
            break;
        }
    }
    osThreadExit();
    (void)argument;
}



static void communication_thread(void * argument)
{
    while (1)
    {
        ret_code_t err_code;
        err_code = thread_communication_run();
        if (err_code != CODE_SUCCESS)
        {
            err_set(sys_app_thread);
            break;
        }

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
    while (1)
    {
        thread_rtos_trace_run(thread_ids);
        if (err_check())
        {
            break;
        }
    }
    app_shutdown();
    osThreadExit();
    (void)argument;
}


static void threads_init(void)
{
    osThreadAttr_t attr = {0};

    attr.name = "blinky";
    attr.priority = osPriorityLow;
    attr.stack_size = 256; // Stack size should be therefore 4 byte aligned in order to avoid division caused side effects.
    thread_ids[THREAD_BLINKY] = osThreadNew(blinky_thread, NULL, &attr);

    attr.name = "dsp";
    attr.priority = osPriorityAboveNormal;
    attr.stack_size = 2048;
    thread_ids[THREAD_DSP] = osThreadNew(dsp_thread, NULL, &attr);

    attr.name = "communication";
    attr.priority = osPriorityAboveNormal;
    attr.stack_size = 1024;
    thread_ids[THREAD_COMMUNICATION] = osThreadNew(communication_thread, NULL, &attr);

    attr.name = "trace";
    attr.priority = osPriorityHigh;
    attr.stack_size = 768;
    thread_ids[THREAD_RTOS_TRACE] = osThreadNew(rtos_trace_thread, NULL, &attr);
}


static void queues_init(void)
{
    osMessageQueueAttr_t attr;

    attr.mq_mem = pool_queue_usb_rx;
    attr.mq_size = sizeof(pool_queue_usb_rx);
    attr.cb_mem = cb_queue[QUEUE_USB_RX];
    attr.cb_size = sizeof(StaticQueue_t);
    queues_ids[QUEUE_USB_RX] = osMessageQueueNew(APP_QUEUE_RX_MSG_COUNT, APP_QUEUE_RX_MSG_SIZE, &attr);

    attr.mq_mem = pool_queue_usb_tx;
    attr.mq_size = sizeof(pool_queue_usb_tx);
    attr.cb_mem = cb_queue[QUEUE_USB_TX];
    attr.cb_size = sizeof(StaticQueue_t);
    queues_ids[QUEUE_USB_TX] = osMessageQueueNew(APP_QUEUE_TX_MSG_COUNT, APP_QUEUE_TX_MSG_SIZE, &attr);
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
    ret_code_t err_code;

    queues_init();
    mutexes_init();
    semaphores_init();
    threads_init();

    thread_rtos_trace_init(&thread_ids[THREAD_RTOS_TRACE]);
    thread_blinky_init();

    thread_communication_init_t thread_comm_init = 
    {
        .p_queue_rx_msg_id = &queues_ids[QUEUE_USB_RX],
        .p_queue_tx_msg_id = &queues_ids[QUEUE_USB_TX],
        .p_wakeup_thread_id = &thread_ids[THREAD_DSP],
        .p_cur_thread_id = &thread_ids[THREAD_COMMUNICATION],
        .flags = THREAD_DSP_WAKEUP_FLAG,
        .stop_stream_cb = thread_dsp_stop_meas_stream_cb,
    };
    err_code = thread_communication_init(&thread_comm_init);
    if (err_code != CODE_SUCCESS)
    {   
        app_shutdown();
        return;
    }

    thread_dsp_init_t thread_dsp_cfg = 
    {
        .p_new_data_queue = &queues_ids[QUEUE_USB_RX]
    };
    err_code = thread_dsp_init(&thread_dsp_cfg);
    if (err_code != CODE_SUCCESS)
    {
        app_shutdown();
        return;
    }

    dbg_printf("<App OS> Initialization successfull\n");
    dbg_printf("<App OS> Application starts.\n");
}