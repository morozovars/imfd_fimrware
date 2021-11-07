#ifndef USBD_CDC_IF_H_
#define USBD_CDC_IF_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "usbd_cdc.h"


typedef enum
{
    CDC_EVT_NEW_DATA_RECEIVED,
    CDC_EVT_TRANSMISSION_COMPLETE,
    CDC_EVT_COUNT,
} cdc_evt_type_t;


typedef struct
{ 
    uint8_t *   p_data;
    uint16_t *  p_length;
} cdc_evt_new_data_t;


typedef struct 
{
    uint8_t *   p_data;
    uint16_t *  p_length;
    uint8_t     epnum;
} cdc_evt_transmission_cmplt_t;


typedef struct
{
    cdc_evt_type_t    evt_type;
    union 
    {
        cdc_evt_new_data_t              new_data;
        cdc_evt_transmission_cmplt_t    transmission_cmplt;
    } evt;
}cdc_evt_params_t;


typedef void (* cdc_evt_handler_t)(cdc_evt_params_t params);


extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

/** @defgroup USBD_CDC_IF_Exported_FunctionsPrototype USBD_CDC_IF_Exported_FunctionsPrototype
  * @brief Public functions declaration.
  * @{
 */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);


void CDC_set_evt_handler(cdc_evt_handler_t p_cb);


#ifdef __cplusplus
}
#endif


#endif // USBD_CDC_IF_H_