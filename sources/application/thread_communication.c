#include "thread_communication.h"
#include "app_initializer.h"
#include "app_config.h"

#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "usbd_core.h"
#include "usbd_desc.h"

#include "trace/dbgout.h"


osMessageQueueId_t  *   p_queue_id;
osThreadId_t *          p_wakeup_thread_id;
uint32_t                flags;

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
                .overflow = (*(params.evt.new_data.p_length) > APP_USBD_BUF_SIZES) ? true : false,
            };
            uint16_t copy_len = (msg.overflow) ? APP_USBD_BUF_SIZES : msg.length;
            memcpy(msg.buf, params.evt.new_data.p_data, copy_len);
            osMessageQueuePut(*p_queue_id, &msg, 0u, 0u);
            osThreadFlagsSet(*p_wakeup_thread_id, flags);
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
      p_queue_id = p_init->p_queue_id;
      p_wakeup_thread_id = p_init->p_wakeup_thread_id;
      flags = p_init->flags;

      ret_code_t err_code = usbd_init();
      return err_code;
}


ret_code_t thread_communication_run(void) { return CODE_SUCCESS; }


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