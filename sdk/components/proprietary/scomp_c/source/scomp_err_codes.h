#ifndef SCOMP_ERR_CODES_H_
#define SCOMP_ERR_CODES_H_

#include <communication_err_codes.h>

#define SCOMP_ERROR_BASE_NUM  COMMUNICATION_ERROR_BASE_NUM - 10u

#define SCOMP_SUCCESS       (COMMUNICATION_SUCCESS)
#define SCOMP_WRONG_ARGS    (SCOMP_ERROR_BASE_NUM - 1u)

typedef comm_err_code_t scomp_err_code_t; 

#endif // ! SCOMP_ERR_CODES_H_