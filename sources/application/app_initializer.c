#include "app_initializer.h"
#include "board_config.h"
#include "gpio.h"
#include "trace/app_trace.h"
#include "trace/dbgout.h"
#include "trace/err_handler.h"
#include <stdbool.h>
#include <stdio.h>

#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "usbd_core.h"
#include "usbd_desc.h"


USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef CDC_Desc;


static inline bool is_err(ret_code_t code)
{
    if (code != CODE_SUCCESS)
    {
        dbg_printf("app_init: err - %d\n", code);
        return true;
    }
    return false;
}


/**
 * @brief USB Clock Configuration
 * @retval None
 */
static void USBD_clock_config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_CRSInitTypeDef RCC_CRSInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return;
    }
    __HAL_RCC_CRS_CLK_ENABLE();
    RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
    RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;
    RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000);
    RCC_CRSInitStruct.ErrorLimitValue = RCC_CRS_ERRORLIMIT_DEFAULT;
    RCC_CRSInitStruct.HSI48CalibrationValue = RCC_CRS_HSI48CALIBRATION_DEFAULT;
    HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);
}


static void system_clock_config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 64;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
    RCC_OscInitStruct.PLL.PLLN = 75;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return;
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        return;
    }
}


static void USBD_clock_resume(void)
{
    system_clock_config();
    USBD_clock_config();
}


ret_code_t usbd_init(void)
{
    USBD_clock_config();

    USBD_register_clock_config_func(USBD_clock_resume);

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

    return CODE_SUCCESS;
}

// Init shared and single use periphery
static ret_code_t cpu_init(void)
{
    ret_code_t err_code;
    err_code = usbd_init();
    if (err_code != CODE_SUCCESS)
    {
        return err_code;
    }
    return CODE_SUCCESS;
}


ret_code_t app_init(void)
{
    ret_code_t err_code;

    system_clock_config();

    HAL_Init();

    err_code = cpu_init();
    if (is_err(err_code))
    {
        return err_code;
    }

    app_trace_init();

    // TODO: not thread safe application initialization...

    dbg_printf("<App Init> Initialization successfull\n");

    return CODE_SUCCESS;
}


static void cpu_uninit(void) { ; }


void app_shutdown(void)
{
    cpu_uninit();
    err_printout();
    dbg_printf("<App shutdown> System shutdown\n");
#ifdef DEBUG
    __asm__("BKPT 0");
#endif
    NVIC_SystemReset();
}