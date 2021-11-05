#ifndef ERR_CODES_H_
#define ERR_CODES_H_


#include <stdint.h>


#define CODE_ERROR_BASE_NUM             0
#define CODE_DRIVER_ERROR_BASE_NUM      -100
#define CODE_LIB_ERROR_BASE_NUM         -200


// Common error codes.
#define CODE_SUCCESS        (CODE_ERROR_BASE_NUM - 0)
#define CODE_ERROR          (CODE_ERROR_BASE_NUM - 1)
#define CODE_TIMEOUT        (CODE_ERROR_BASE_NUM - 2)
#define CODE_BUSY           (CODE_ERROR_BASE_NUM - 3)
#define CODE_NO_MEM         (CODE_ERROR_BASE_NUM - 4)
#define CODE_NOT_FOUND      (CODE_ERROR_BASE_NUM - 5)
#define CODE_NULL           (CODE_ERROR_BASE_NUM - 6)
#define CODE_INVALID_PARAM  (CODE_ERROR_BASE_NUM - 7)
#define CODE_FORBIDDEN      (CODE_ERROR_BASE_NUM - 8)
#define CODE_RESOURCES      (CODE_ERROR_BASE_NUM - 9)
#define CODE_OS_ISR         (CODE_ERROR_BASE_NUM - 10)

// Common other codes.
#define CODE_NEW_DATA       (CODE_ERROR_BASE_NUM + 1)

typedef int32_t ret_code_t;


static inline ret_code_t hal2code(uint32_t code)
{
  switch (code)
  {
    case 0:
      return CODE_SUCCESS;
    case 1:
      return CODE_ERROR;
    case 2:
      return CODE_BUSY;
    case 3:
      return CODE_TIMEOUT;
    default:
      break;
  }
  return CODE_SUCCESS;
}


static inline ret_code_t os_status2code(int32_t code)
{
    switch (code)
    {
        case 0:
            return CODE_SUCCESS;
        case -1:
            return CODE_ERROR;
        case -2:
            return CODE_TIMEOUT;
        case -3:
            return CODE_RESOURCES;
        case -4:
            return CODE_INVALID_PARAM;
        case -5:
            return CODE_NO_MEM;
        case -6:
            return CODE_OS_ISR;
        default:
          break;
    }

    return CODE_SUCCESS;
}


#define CHECK_EXPR(_expr, _err)    if (_expr) return _err

#endif