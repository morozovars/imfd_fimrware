#include "app_initializer.h"
#include "board_config.h"
#include "gpio.h"
#include "trace/app_trace.h"
#include "trace/dbgout.h"
#include "trace/err_handler.h"
#include <stdbool.h>
#include <stdio.h>


static inline bool is_err(ret_code_t code)
{
    if (code != CODE_SUCCESS)
    {
        dbg_printf("app_init: err - %d\n", code);
        return true;
    }
    return false;
}


ret_code_t app_cpu_clock_config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /// Configure main internal regulator output voltage.
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK)
    {
        return CODE_ERROR;
    }

    /// Initializes the RCC Oscillators according to the specified parameters
    /// in the RCC_OscInitTypeDef structure.
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
    RCC_OscInitStruct.PLL.PLLN = 85;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV8;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV8;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return CODE_ERROR;
    }
    /// Initializes the CPU, AHB and APB buses clocks.
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        return CODE_ERROR;
    }
    /// Initializes the peripherals clocks.
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        return CODE_ERROR;
    }

    return CODE_SUCCESS;
}

// Init shared and single use periphery
static ret_code_t cpu_init(void)
{
    ret_code_t err_code;
    err_code = app_cpu_clock_config();
    if (is_err(err_code))
    {
        return err_code;
    }
    return CODE_SUCCESS;
}


ret_code_t app_init(void)
{
    ret_code_t err_code;
    err_code = hal2code(HAL_Init());
    if (is_err(err_code))
    {
        return err_code;
    }
    err_code = cpu_init();
    if (is_err(err_code))
    {
        return err_code;
    }
    dbg_printf("<App Init> Initialization successfull\n");

    dbg_printf("<App Init> PCLK1 freq = %d Hz\n", HAL_RCC_GetPCLK1Freq());
    dbg_printf("<App Init> PCLK2 freq = %d Hz\n", HAL_RCC_GetPCLK2Freq());
    dbg_printf("<App Init> SysClock freq = %d Hz\n", HAL_RCC_GetSysClockFreq());

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