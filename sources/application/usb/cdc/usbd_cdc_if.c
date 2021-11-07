#include "usbd_cdc_if.h"
#include "app_config.h"

uint8_t UserRxBufferFS[APP_USBD_BUF_SIZES];
uint8_t UserTxBufferFS[APP_USBD_BUF_SIZES];

extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t * pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t * pbuf, uint32_t * Len);
static int8_t CDC_TransmitCplt_FS(uint8_t * pbuf, uint32_t * Len, uint8_t epnum);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = {
    CDC_Init_FS, CDC_DeInit_FS, CDC_Control_FS, CDC_Receive_FS, CDC_TransmitCplt_FS};


static cdc_evt_handler_t p_callback = NULL;

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initializes the CDC media low layer over the FS USB IP
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t CDC_Init_FS(void)
{
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);

    return (USBD_OK);
}

/**
 * @brief  DeInitializes the CDC media low layer
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t CDC_DeInit_FS(void) { return (USBD_OK); }

/**
 * @brief  Manage the CDC class requests
 * @param  cmd: Command code
 * @param  pbuf: Buffer containing command data (request parameters)
 * @param  length: Number of data to be sent (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t * pbuf, uint16_t length)
{
    /* USER CODE BEGIN 5 */
    switch (cmd)
    {
        case CDC_SEND_ENCAPSULATED_COMMAND:
            /* Add your code here */
            break;

        case CDC_GET_ENCAPSULATED_RESPONSE:
            /* Add your code here */
            break;

        case CDC_SET_COMM_FEATURE:
            /* Add your code here */
            break;

        case CDC_GET_COMM_FEATURE:
            /* Add your code here */
            break;

        case CDC_CLEAR_COMM_FEATURE:
            /* Add your code here */
            break;

        case CDC_SET_LINE_CODING:
            /* Add your code here */
            break;

        case CDC_GET_LINE_CODING:
            /* Add your code here */
            break;

        case CDC_SET_CONTROL_LINE_STATE:
            /* Add your code here */
            break;

        case CDC_SEND_BREAK:
            /* Add your code here */
            break;

        default:
            break;
    }
    (void)pbuf;
    (void)length;
    return (USBD_OK);
    /* USER CODE END 5 */
}

/**
 * @brief  Data received over USB OUT endpoint are sent over CDC interface
 *         through this function.
 *
 *         @note
 *         This function will issue a NAK packet on any OUT packet received on
 *         USB endpoint until exiting this function. If you exit this function
 *         before transfer is complete on CDC interface (ie. using DMA controller)
 *         it will result in receiving more data while previous ones are still
 *         not sent.
 *
 * @param  Buf: Buffer of data to be received
 * @param  Len: Number of data received (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t CDC_Receive_FS(uint8_t * Buf, uint32_t * Len)
{
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    if (p_callback != NULL)
    {
        cdc_evt_new_data_t new_data_params = 
        {
            .p_data = Buf,
            .p_length = (uint16_t *)Len
        };
        cdc_evt_params_t params = 
        {
            .evt_type = CDC_EVT_NEW_DATA_RECEIVED,
            .evt.new_data = new_data_params,
        };
        p_callback(params);
    }    
    return (USBD_OK);
}

/**
 * @brief  CDC_Transmit_FS
 *         Data to send over USB IN endpoint are sent over CDC interface
 *         through this function.
 *         @note
 *
 *
 * @param  Buf: Buffer of data to be sent
 * @param  Len: Number of data to be sent (in bytes)
 * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
 */
uint8_t CDC_Transmit_FS(uint8_t * Buf, uint16_t Len)
{
    uint8_t result = USBD_OK;
    USBD_CDC_HandleTypeDef * hcdc = (USBD_CDC_HandleTypeDef *)hUsbDeviceFS.pClassData;
    if (hcdc->TxState != 0)
    {
        return USBD_BUSY;
    }
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
    result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    return result;
}

/**
 * @brief  CDC_TransmitCplt_FS
 *         Data transmited callback
 *
 *         @note
 *         This function is IN transfer complete callback used to inform user that
 *         the submitted Data is successfully sent over USB.
 *
 * @param  Buf: Buffer of data to be received
 * @param  Len: Number of data received (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t CDC_TransmitCplt_FS(uint8_t * Buf, uint32_t * Len, uint8_t epnum)
{
    uint8_t result = USBD_OK;
    if (p_callback != NULL)
    {
        cdc_evt_transmission_cmplt_t transmission_cmplt_data = 
        {
            .p_data = Buf,
            .p_length = (uint16_t *)Len,
            .epnum = epnum
        };
        cdc_evt_params_t params =
        {
            .evt_type = CDC_EVT_TRANSMISSION_COMPLETE,
            .evt.transmission_cmplt = transmission_cmplt_data
        };
        p_callback(params);
    }
    UNUSED(Buf);
    UNUSED(Len);
    UNUSED(epnum);
    return result;
}



void CDC_set_evt_handler(cdc_evt_handler_t p_cb)
{
    if (p_cb != NULL)
    {
        p_callback = p_cb;
    }
}