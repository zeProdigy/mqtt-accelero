#include <assert.h>
#include "osal/osal.h"


osal_semaphore_t osal_semaphore_create(void)
{
    osal_semaphore_t sem = xSemaphoreCreateBinary();
    if (sem != NULL) {
        // В FreeRTOS после создания семафора он находится в активном состоянии
        // это не удобно
        osal_semaphore_wait(sem, 0);
    }

    return sem;
}


int osal_semaphore_release(osal_semaphore_t sem)
{
    assert(sem);

    if (IN_ISR()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        BaseType_t ret = xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        return (ret == pdPASS) ? 0 : 1;
    } else {
        return (xSemaphoreGive(sem) == pdPASS) ? 0 : 1;
    }
}


int osal_semaphore_wait(osal_semaphore_t sem, uint32_t ms_timeout)
{
    assert(sem);

    uint32_t const ticks = (ms_timeout == OSAL_MAX_TIMEOUT) ?
        portMAX_DELAY :
        pdMS_TO_TICKS(ms_timeout);

    return (xSemaphoreTake(sem, ticks) == pdPASS) ? 0 : 1;
}


int osal_semaphore_delete(osal_semaphore_t sem)
{
    assert(sem);
    vSemaphoreDelete(sem);
    return 0;
}
