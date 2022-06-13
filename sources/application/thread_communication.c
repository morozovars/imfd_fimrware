#include "thread_communication.h"
#include "app_config.h"
#include "app_initializer.h"
#include "fft_sfm.h"

#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include <inttypes.h>

#include "trace/dbgout.h"

#include "stm32g4xx_hal.h"


#define APP_PRINTF(...)                                                                            \
    dbg_printf("(%d) ", osKernelGetTickCount());                                                   \
    dbg_printf("<USBD> ");                                                                         \
    dbg_printf(__VA_ARGS__);                                                                       \
    dbg_printf("\n");


typedef enum
{
    THREAD_COMMUNICATION_FLAG_SET_FREQ = 0x01,
    THREAD_COMMUNICATION_FLAG_SET_MEAS = 0x02,
    THREAD_COMMUNICATION_FLAG_MEAS = THREAD_COMMUNICATION_FLAG_SET_MEAS * 2,
    THREAD_COMMUNICATION_FLAG_USE_DEFAULT_REF_GMV = THREAD_COMMUNICATION_FLAG_MEAS * 2,
    THREAD_COMMUNICATION_FLAG_USE_CALIB_REF_GMV = THREAD_COMMUNICATION_FLAG_USE_DEFAULT_REF_GMV * 2,
    THREAD_COMMUNICATION_FLAG_USE_CUR_GMV_AS_REF_GMV =
        THREAD_COMMUNICATION_FLAG_USE_CALIB_REF_GMV * 2,
    THREAD_COMMUNICATION_FLAG_STORE_CUR_GMV_AS_CALIB =
        THREAD_COMMUNICATION_FLAG_USE_CUR_GMV_AS_REF_GMV * 2,
    THREAD_COMMUNICATION_FLAG_CHANGE_IFR_OF_DEBUG =
        THREAD_COMMUNICATION_FLAG_STORE_CUR_GMV_AS_CALIB * 2,
    THREAD_COMMUNICATION_FLAG_TX_DATA_READY = THREAD_COMMUNICATION_FLAG_CHANGE_IFR_OF_DEBUG * 2,
    THREAD_COMMUNICATION_FLAG_COUNT,
} flags_t;


enum
{
    TEMPORARY_TX_BUF_ENQUEUE_IDX = 0u,
    TEMPORARY_TX_BUF_DEQUEUE_IDX,
    TEMPORARY_TX_BUF_COUNT,
};


static osMessageQueueId_t * p_queue_usb_rx;
static osMessageQueueId_t * p_queue_usb_tx;
static osThreadId_t * p_wakeup_thread_id;
static uint32_t wakeup_flags;
static osThreadId_t * p_cur_thread_id;
static void (*p_stop_stream_cb)(void);
static bool is_meas_buf = false;
static uint64_t total_stream_data_size = 0u;
static osTimerId_t timer_handle;
static app_queue_tx_msg_t temporary_tx_msg[TEMPORARY_TX_BUF_COUNT];


USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef CDC_Desc;


static void timer_cb(void * argument)
{
    APP_PRINTF("Stream stopped, total data = %" PRIu64 " bytes", total_stream_data_size);
    total_stream_data_size = 0u;
    p_stop_stream_cb();
    (void)argument;
}


static void cdc_evt_handler(cdc_evt_params_t params)
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
                    osThreadFlagsSet(*p_cur_thread_id, THREAD_COMMUNICATION_FLAG_MEAS);
                    total_stream_data_size += meas_buf_length_cur;
                    total_stream_data_size += COMMUNICATION_HEADER_TOTAL_SIZE;
                }
            }
            else
            {
                process_length -= COMMUNICATION_HEADER_TOTAL_SIZE;
                switch (p_rx[0])
                {
                    case COMMUNICATION_MSG_TYPE_MEASURE:
                        is_meas_buf = true;
                        meas_buf_length_target = *(uint32_t *)&p_rx[4];
                        meas_buf_length_cur = process_length;
                        osThreadFlagsSet(*p_wakeup_thread_id, wakeup_flags);
                        break;
                    case COMMUNICATION_MSG_TYPE_SET_FREQ:
                        osThreadFlagsSet(*p_cur_thread_id, THREAD_COMMUNICATION_FLAG_SET_FREQ);
                        break;
                    case COMMUNICATION_MSG_TYPE_SET_MEAS_TYPE:
                        osThreadFlagsSet(*p_cur_thread_id, THREAD_COMMUNICATION_FLAG_SET_MEAS);
                        break;
                    case COMMUNICATION_MSG_TYPE_USE_DEFAULT_REF_GMV:
                        osThreadFlagsSet(
                            *p_cur_thread_id, THREAD_COMMUNICATION_FLAG_USE_DEFAULT_REF_GMV);
                        break;
                    case COMMUNICATION_MSG_TYPE_USE_CALIB_REF_GMV:
                        osThreadFlagsSet(
                            *p_cur_thread_id, THREAD_COMMUNICATION_FLAG_USE_CALIB_REF_GMV);
                        break;
                    case COMMUNICATION_MSG_TYPE_USE_CUR_GMV_AS_REF_GMV:
                        osThreadFlagsSet(
                            *p_cur_thread_id, THREAD_COMMUNICATION_FLAG_USE_CUR_GMV_AS_REF_GMV);
                        break;
                    case COMMUNICATION_MSG_TYPE_STORE_CUR_GMV_AS_CALIB:
                        osThreadFlagsSet(
                            *p_cur_thread_id, THREAD_COMMUNICATION_FLAG_STORE_CUR_GMV_AS_CALIB);
                        break;
                    case COMMUNICATION_MSG_TYPE_CHANGE_IFR_OF_DEBUG:
                        osThreadFlagsSet(
                            *p_cur_thread_id, THREAD_COMMUNICATION_FLAG_CHANGE_IFR_OF_DEBUG);
                    default:
                        break;
                }
                p_rx = p_rx + COMMUNICATION_HEADER_TOTAL_SIZE;
            }
            if (process_length > 0u)
            {
                app_queue_rx_msg_t msg;
                memcpy(msg.buf, p_rx, process_length);
                msg.length = process_length;
                osMessageQueuePut(*p_queue_usb_rx, &msg, 0u, 0u);
            }
            break;
        }
        default:
            break;
    }
}


static ret_code_t usbd_init(void)
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


static uint32_t get_bank(uint32_t Addr)
{
    uint32_t bank = 0;

    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
    {
        /* No Bank swap */
        bank = (Addr < (FLASH_BASE + FLASH_BANK_SIZE)) ? FLASH_BANK_1 : FLASH_BANK_2;
    }
    else
    {
        /* Bank swap */
        bank = (Addr < (FLASH_BASE + FLASH_BANK_SIZE)) ? FLASH_BANK_2 : FLASH_BANK_1;
    }

    return bank;
}


static int store_ref_gmv_on_flash(void)
{
    POINT_PRECISION * p_data;
    uint32_t length;
    uint32_t addr, addr_end;
    uint32_t page_error;
    FLASH_EraseInitTypeDef erase_init;
    imfd_meas_type_t meas_type;

    fft_sfm_get_ref_gmv(&p_data, &length);
    fft_sfm_get_meas_type(&meas_type);

    if (meas_type == IMFD_MEAS_SINGLE_CURRENT)
    {
        addr = APP_FLASH_ADDR_CALIB_REG_GMV_CURRENT;
    }
    else if (meas_type == IMFD_MEAS_VIB_RADIAL)
    {
        addr = APP_FLASH_ADDR_CALIB_REG_GMV_VIB1;
    }
    else if (meas_type == IMFD_MEAS_VIB_AXIAL)
    {
        addr = APP_FLASH_ADDR_CALIB_REG_GMV_VIB2;
    }
    addr_end = addr + length;
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
    erase_init.Banks = get_bank(addr);
    erase_init.TypeErase = FLASH_TYPEERASE_MASSERASE;
    if (HAL_FLASHEx_Erase(&erase_init, &page_error) != HAL_OK)
    {
        APP_PRINTF("Erase error, 0x%08X", page_error);
        HAL_FLASH_Lock();
        return -1;
    }
    while (addr_end > addr)
    {
        uint64_t mem_data;
        memcpy(&mem_data, p_data, sizeof(uint64_t));
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, mem_data) != HAL_OK)
        {
            APP_PRINTF("FLASH program error, addr = %d", (uint32_t)p_data);
            HAL_FLASH_Lock();
            return -1;
        }
        addr += sizeof(POINT_PRECISION);
        p_data += 1;
    }
    HAL_FLASH_Lock();
    return 0;
}


ret_code_t thread_communication_init(thread_communication_init_t * p_init)
{
    p_queue_usb_rx = p_init->p_queue_rx_msg_id;
    p_queue_usb_tx = p_init->p_queue_tx_msg_id;
    p_wakeup_thread_id = p_init->p_wakeup_thread_id;
    p_cur_thread_id = p_init->p_cur_thread_id;
    wakeup_flags = p_init->flags;
    p_stop_stream_cb = p_init->stop_stream_cb;

    ret_code_t err_code = usbd_init();

    total_stream_data_size = 0u;
    timer_handle = osTimerNew(timer_cb, osTimerOnce, NULL, NULL);

    return err_code;
}


ret_code_t thread_communication_run(void)
{
    app_queue_rx_msg_t msg;
    ret_code_t ret_code;

    uint32_t flag = osThreadFlagsWait(
        THREAD_COMMUNICATION_FLAG_MEAS | THREAD_COMMUNICATION_FLAG_SET_FREQ |
            THREAD_COMMUNICATION_FLAG_SET_MEAS | THREAD_COMMUNICATION_FLAG_USE_DEFAULT_REF_GMV |
            THREAD_COMMUNICATION_FLAG_USE_CALIB_REF_GMV |
            THREAD_COMMUNICATION_FLAG_USE_CUR_GMV_AS_REF_GMV |
            THREAD_COMMUNICATION_FLAG_STORE_CUR_GMV_AS_CALIB |
            THREAD_COMMUNICATION_FLAG_CHANGE_IFR_OF_DEBUG | THREAD_COMMUNICATION_FLAG_TX_DATA_READY,
        osFlagsWaitAny, osWaitForever);

    if (flag >= osFlagsErrorISR)
    {
        return CODE_ERROR;
    }

    if (flag == THREAD_COMMUNICATION_FLAG_MEAS)
    {
        osTimerStop(timer_handle);
        osTimerStart(timer_handle, 5000u);
        // NOTE: just to notify this thread.
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
        memcpy(&sample_freq, msg.buf, sizeof(sample_freq));
        APP_PRINTF("Update Fs, new Fs = %d", sample_freq);
        fft_sfm_set_fs(sample_freq);
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
        uint8_t meas_type = msg.buf[0];
        switch (meas_type)
        {
            case COMMUNICATION_MEAS_TYPE_CURRENT:
                APP_PRINTF("Set meas, new type - Current");
                fft_sfm_set_meas_type(IMFD_MEAS_SINGLE_CURRENT);
                break;
            case COMMUNICATION_MEAS_TYPE_VIB_RADIAL:
                APP_PRINTF("Set meas, new type - Vibration Radial");
                fft_sfm_set_meas_type(IMFD_MEAS_VIB_RADIAL);
                break;
            case COMMUNICATION_MEAS_TYPE_VIB_AXIAL:
                APP_PRINTF("Set meas, new type - Vibration Axial");
                fft_sfm_set_meas_type(IMFD_MEAS_VIB_AXIAL);
                break;
            default:
                APP_PRINTF("Set meas, received unknown type. Ignored.");
                break;
        };
    }
    if (flag == THREAD_COMMUNICATION_FLAG_USE_DEFAULT_REF_GMV)
    {
        fft_sfm_set_ref_gmv(IMFD_REF_GMV_LOAD_DEFAULT);
        APP_PRINTF("Load default GMV.");
    }
    if (flag == THREAD_COMMUNICATION_FLAG_USE_CALIB_REF_GMV)
    {
        fft_sfm_set_ref_gmv(IMFD_REF_GMV_LOAD_CALIB);
        APP_PRINTF("Load calib GMV."); // TODO:load GMV.
    }
    if (flag == THREAD_COMMUNICATION_FLAG_USE_CUR_GMV_AS_REF_GMV)
    {
        fft_sfm_set_ref_gmv(IMFD_REF_GMV_LOAD_FROM_CURRENT);
        APP_PRINTF("Set current GMV as reference.");
    }
    if (flag == THREAD_COMMUNICATION_FLAG_STORE_CUR_GMV_AS_CALIB)
    {
        store_ref_gmv_on_flash();
        APP_PRINTF("Store current reference GMV to FLASH.");
    }
    if (flag == THREAD_COMMUNICATION_FLAG_CHANGE_IFR_OF_DEBUG)
    {
        ret_code = os_status2code(osMessageQueueGet(*p_queue_usb_rx, &msg, NULL, 0u));
        if (ret_code != CODE_SUCCESS)
        {
            APP_PRINTF("Change IFR, queue error = %d", ret_code);
            return CODE_ERROR;
        }
        if (msg.length != sizeof(uint8_t))
        {
            APP_PRINTF("Change IFR length error, length = %d", msg.length);
            return CODE_ERROR;
        }
        uint8_t ifr = msg.buf[0];
        APP_PRINTF("Change IFR of debug = %d", ifr);
        fft_sfm_change_ifr_of_debug(ifr);
    }
    if (flag == THREAD_COMMUNICATION_FLAG_TX_DATA_READY)
    {
        uint8_t tx_msg_count = osMessageQueueGetCount(*p_queue_usb_tx);
        uint8_t * p_dequeue_data = (uint8_t *)&temporary_tx_msg[TEMPORARY_TX_BUF_DEQUEUE_IDX].data;
        uint16_t * p_dequeue_data_size =
            (uint16_t *)&temporary_tx_msg[TEMPORARY_TX_BUF_DEQUEUE_IDX].length;
        while (tx_msg_count != 0)
        {
            osDelay(250u);
            ret_code = os_status2code(osMessageQueueGet(
                *p_queue_usb_tx, &temporary_tx_msg[TEMPORARY_TX_BUF_DEQUEUE_IDX], 0u, 0u));
            if (ret_code == CODE_RESOURCES)
            {
                return CODE_SUCCESS;
            }
            else if (ret_code != CODE_SUCCESS)
            {
                APP_PRINTF("USBD TX queue error");
                return ret_code;
            }
            int8_t usbd_code = CDC_Transmit_FS(p_dequeue_data, *p_dequeue_data_size);
            if (usbd_code != USBD_OK)
            {
                APP_PRINTF("USBD transfer error");
                return CODE_BUSY;
            }

            tx_msg_count--;
        }
    }
    return CODE_SUCCESS;
}


ret_code_t thread_communication_transmit(
    communication_ret_msg_type_t type, uint8_t * p_buf, uint16_t len)
{
    osStatus_t status = osOK;
    uint8_t * p_tx_enqueue_data = (uint8_t *)&temporary_tx_msg[TEMPORARY_TX_BUF_ENQUEUE_IDX].data;
    uint16_t * p_tx_enqueue_size =
        (uint16_t *)&temporary_tx_msg[TEMPORARY_TX_BUF_ENQUEUE_IDX].length;

    if ((len + COMMUNICATION_HEADER_TOTAL_SIZE) > APP_USBD_TX_BUF_SIZE)
    {
        return CODE_RESOURCES;
    }

    memset(p_tx_enqueue_data, 0, len + COMMUNICATION_HEADER_TOTAL_SIZE);

    p_tx_enqueue_data[COMMUNICATION_HEADER_TYPE_IDX] = type;
    memcpy(p_tx_enqueue_data + COMMUNICATION_HEADER_LEN_IDX, &len, sizeof(len));
    memcpy(p_tx_enqueue_data + COMMUNICATION_HEADER_TOTAL_SIZE, p_buf, len);
    *p_tx_enqueue_size = len + COMMUNICATION_HEADER_TOTAL_SIZE;
    status =
        osMessageQueuePut(*p_queue_usb_tx, &temporary_tx_msg[TEMPORARY_TX_BUF_ENQUEUE_IDX], 0u, 0u);
    if (status != osOK)
    {
        return os_status2code(status);
    }

    osThreadFlagsSet(*p_cur_thread_id, THREAD_COMMUNICATION_FLAG_TX_DATA_READY);

    return CODE_SUCCESS;
}