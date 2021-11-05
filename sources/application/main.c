#include "app_initializer.h"
#include "app_os.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <stdlib.h>


int main(void)
{
    ret_code_t err_code = app_init();
    if (err_code != CODE_SUCCESS)
    {
        app_shutdown();
        return 0;
    }

    osKernelInitialize();
    app_os_init();
    osKernelStart();

    while (1)
        ;
}