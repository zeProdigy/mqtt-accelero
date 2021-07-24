#include "osal/osal.h"


extern void xPortSysTickHandler(void);


int osal_kernel_start(void)
{
    vTaskStartScheduler();
    return 0;
}


int osal_kernel_systick_handler(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
    return 0;
}


size_t osal_kernel_get_free_heap_size(void)
{
    return xPortGetFreeHeapSize();
}
