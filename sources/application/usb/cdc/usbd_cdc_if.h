#ifndef USBD_CDC_IF_H_
#define USBD_CDC_IF_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "usbd_cdc.h"


extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

/** @defgroup USBD_CDC_IF_Exported_FunctionsPrototype USBD_CDC_IF_Exported_FunctionsPrototype
  * @brief Public functions declaration.
  * @{
 */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);


#ifdef __cplusplus
}
#endif


#endif // USBD_CDC_IF_H_