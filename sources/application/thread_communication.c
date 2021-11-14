#include "thread_communication.h"
#include "app_initializer.h"
#include "app_config.h"

#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "usbd_core.h"
#include "usbd_desc.h"

#include "trace/dbgout.h"


typedef enum
{
    THREAD_COMMUNICATION_FLAG_DRDY = 0x01,
    THREAD_COMMUNICATION_FLAG_COUNT,
} flags_t;


static osMessageQueueId_t  *   p_queue_usb_rx;
static osThreadId_t *          p_wakeup_thread_id;
static uint32_t                wakeup_flags;
static osMessageQueueId_t  *   p_queue_meas;
static osThreadId_t *          p_cur_thread_id;

USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef CDC_Desc;


void cdc_evt_handler(cdc_evt_params_t params)
{
    switch (params.evt_type)
    {
        case CDC_EVT_NEW_DATA_RECEIVED:
        {
            app_queue_rx_msg_t msg = 
            {
                .length = *(params.evt.new_data.p_length),
            };
            memcpy(msg.buf, params.evt.new_data.p_data, msg.length);
            if (os_status2code(osMessageQueuePut(*p_queue_usb_rx, &msg, 0u, 0u)) != CODE_SUCCESS)
            {
                __asm__("BKPT 0");
            }
            osThreadFlagsSet(*p_cur_thread_id, THREAD_COMMUNICATION_FLAG_DRDY);
            break;
        }
        default:
            break;
    }
}


ret_code_t usbd_init(void)
{
    USBD_register_clock_config_func(app_cpu_clock_config);

    if (USBD_Init(&hUsbDeviceFS, &CDC_Desc, DEVICE_FS) != USBD_OK)
    {
        return CODE_RESOURCES;
    }
    if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK)
    {
        return CODE_RESOURCES;
    }
    if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK)
    {
        return CODE_RESOURCES;
    }
    if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
    {
        return CODE_RESOURCES;
    }

    CDC_set_evt_handler(cdc_evt_handler);

    return CODE_SUCCESS;
}


ret_code_t thread_communication_init(thread_communication_init_t * p_init)
{
      p_queue_usb_rx = p_init->p_queue_msg_id;
      p_wakeup_thread_id = p_init->p_wakeup_thread_id;
      p_queue_meas = p_init->p_queue_meas_id;
      p_cur_thread_id = p_init->p_cur_thread_id;
      wakeup_flags = p_init->flags;

      ret_code_t err_code = usbd_init();
      return err_code;
}


uint32_t sum;
uint32_t thousand = 1000;
uint8_t meas_buf[8192];
ret_code_t thread_communication_run(void)
{
    app_queue_rx_msg_t msg;
    app_queue_usb_meas_msg_t meas;
    uint16_t count;
    static uint16_t meas_length = 0;


    uint32_t flag = osThreadFlagsWait(THREAD_COMMUNICATION_FLAG_DRDY, osFlagsWaitAny, osWaitForever);

    if (flag >= osFlagsErrorISR)
    {
        return CODE_ERROR;
    }

    if (flag == THREAD_COMMUNICATION_FLAG_DRDY)
    {
        count = osMessageQueueGetCount(*p_queue_usb_rx);
        dbg_printf("rx queue count = %d\n", count);
        while (count)
        {
            if (os_status2code(osMessageQueueGet(*p_queue_usb_rx, &msg, 0u, 0u)) != CODE_SUCCESS)
            {
                return CODE_ERROR;
            }
            if ((sum + msg.length) < 8192)
            {
                memcpy(&meas_buf[sum], msg.buf, msg.length);
                sum+= msg.length;
            }
            if (sum > thousand)
            {
                dbg_printf("sum = %d\n", sum);
                thousand += 1000u;
            }
            //memcpy(&meas.data, msg.buf, sizeof(meas.data));
            //osMessageQueuePut(*p_queue_meas, &meas, 0u, 0u);
            //osThreadFlagsSet(*p_wakeup_thread_id, wakeup_flags);
            count--;
        }
    }
    return CODE_SUCCESS;
}


ret_code_t thread_communication_transmit(uint8_t * p_buf, uint16_t len)
{
    int8_t usbd_code;
    usbd_code = CDC_Transmit_FS(p_buf, len);
    if (usbd_code != USBD_OK)
    {
        return CODE_BUSY;
    }
    return CODE_SUCCESS;
}