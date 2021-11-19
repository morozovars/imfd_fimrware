#include "thread_communication.h"
#include "app_config.h"
#include "app_initializer.h"

#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "usbd_core.h"
#include "usbd_desc.h"

#include "trace/dbgout.h"


#define APP_PRINTF(...)                                                                             \
                                    dbg_printf("<USBD> ");                                          \
                                    dbg_printf(__VA_ARGS__);                                        \
                                    dbg_printf("\n");



typedef enum
{
    THREAD_COMMUNICATION_FLAG_SET_FREQ = 0x01,
    THREAD_COMMUNICATION_FLAG_SET_MEAS = 0x02,
    THREAD_COMMUNICATION_FLAG_MEAS = THREAD_COMMUNICATION_FLAG_SET_MEAS * 2,
    THREAD_COMMUNICATION_FLAG_COUNT,
} flags_t;


static osMessageQueueId_t * p_queue_usb_rx;
static osThreadId_t * p_wakeup_thread_id;
static uint32_t wakeup_flags;
static osThreadId_t * p_cur_thread_id;
static bool is_meas_buf = false;

USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef CDC_Desc;


void cdc_evt_handler(cdc_evt_params_t params)
{
    uint8_t * p_rx = params.evt.new_data.p_data;
    uint8_t process_length = *params.evt.new_data.p_length;
    static uint32_t meas_buf_length_target = 0u;
    static uint32_t meas_buf_length_cur = 0u;

    switch (params.evt_type)
    {
        case CDC_EVT_NEW_DATA_RECEIVED:
        {
            if (is_meas_buf)
            {
                meas_buf_length_cur += process_length;
                osThreadFlagsSet(*p_wakeup_thread_id, wakeup_flags);
                if (meas_buf_length_cur >= meas_buf_length_target)
                {
                    is_meas_buf = false;
                }
            }
            else
            {
                process_length -= COMMUNICATION_HEADER_LENGTH;
                switch (p_rx[0])
                {
                    case COMMUNICATION_DATA_TYPE_MEASURE:
                        is_meas_buf = true;
                        meas_buf_length_target = *(uint32_t *)&p_rx[4];
                        meas_buf_length_cur = process_length;
                        osThreadFlagsSet(*p_wakeup_thread_id, wakeup_flags);
                        osThreadFlagsSet(
                            *p_cur_thread_id, THREAD_COMMUNICATION_FLAG_MEAS); // just to let know.
                        break;
                    case COMMUNICATION_DATA_TYPE_SET_FREQ:
                        osThreadFlagsSet(*p_cur_thread_id, THREAD_COMMUNICATION_FLAG_SET_FREQ);
                        break;
                    case COMMUNICATION_DATA_TYPE_SET_MEAS_TYPE:
                        osThreadFlagsSet(*p_cur_thread_id, THREAD_COMMUNICATION_FLAG_SET_MEAS);
                        break;
                }
                p_rx = p_rx + COMMUNICATION_HEADER_LENGTH;
            }
            app_queue_rx_msg_t msg;
            memcpy(msg.data.buf, p_rx, process_length);
            msg.length = process_length;
            osMessageQueuePut(*p_queue_usb_rx, &msg, 0u, 0u);
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
    p_cur_thread_id = p_init->p_cur_thread_id;
    wakeup_flags = p_init->flags;

    ret_code_t err_code = usbd_init();

    return err_code;
}


ret_code_t thread_communication_run(void)
{
    app_queue_rx_msg_t msg;
    ret_code_t ret_code;

    uint32_t flag =
        osThreadFlagsWait(THREAD_COMMUNICATION_FLAG_MEAS | THREAD_COMMUNICATION_FLAG_SET_FREQ |
                              THREAD_COMMUNICATION_FLAG_SET_MEAS,
            osFlagsWaitAny, osWaitForever);

    if (flag >= osFlagsErrorISR)
    {
        return CODE_ERROR;
    }

    if (flag == THREAD_COMMUNICATION_FLAG_MEAS)
    {

        //NOTE: just to notify this thread.
    }
    if (flag == THREAD_COMMUNICATION_FLAG_SET_FREQ)
    {
        ret_code = os_status2code(osMessageQueueGet(*p_queue_usb_rx, &msg, NULL, 0u));
        if (ret_code != CODE_SUCCESS)
        {
            APP_PRINTF("Update Fs, queue error = %d", ret_code);
            return CODE_ERROR;
        }
        if (msg.length != sizeof(uint32_t))
        {
            APP_PRINTF("Update Fs, length = %d", msg.length);
            return CODE_ERROR;
        }
        uint32_t sample_freq;
        memcpy(&sample_freq, msg.data.buf, sizeof(sample_freq));
        APP_PRINTF("Update Fs, new Fs = %d", sample_freq);
    }
    if (flag == THREAD_COMMUNICATION_FLAG_SET_MEAS)
    {
        ret_code = os_status2code(osMessageQueueGet(*p_queue_usb_rx, &msg, NULL, 0u));
        if (ret_code != CODE_SUCCESS)
        {
            APP_PRINTF("Set meas, queue error = %d", ret_code);
            return CODE_ERROR;
        }
        if (msg.length != sizeof(uint8_t))
        {
            APP_PRINTF("Set meas, length = %d", msg.length);
            return CODE_ERROR;
        }
        uint8_t meas_type = msg.data.buf[0];
        switch (meas_type)
        {
            case COMMUNICATION_MEAS_TYPE_CURRENT:
                APP_PRINTF("Set meas, new type - Current");
                break;
            case COMMUNICATION_MEAS_TYPE_VIB_AXIAL:
                APP_PRINTF("Set meas, new type - Vibration Axial");
                break;
            case COMMUNICATION_MEAS_TYPE_VIB_VERTICAL:
                APP_PRINTF("Set meas, new type - Vibration Vertical");
                break;
            default:
                APP_PRINTF("Set meas, received unknown type. Ignored.");
                break;
        };
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