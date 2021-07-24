#include <assert.h>
#include "osal/osal.h"


osal_mutex_t osal_mutex_create(void)
{
    return xSemaphoreCreateMutex();
}


int osal_mutex_lock(osal_mutex_t mutex, uint32_t ms_timeout)
{
    assert(mutex);

    uint32_t const ticks = (ms_timeout == OSAL_MAX_TIMEOUT) ?
        portMAX_DELAY :
        pdMS_TO_TICKS(ms_timeout);

    return (xSemaphoreTake(mutex, ticks) == pdPASS) ? 0 : 1;
}


int osal_mutex_unlock(osal_mutex_t mutex)
{
    assert(mutex);

    if (IN_ISR()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        BaseType_t ret = xSemaphoreGiveFromISR(mutex, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        return (ret == pdPASS) ? 0 : 1;
    } else {
        return (xSemaphoreGive(mutex) == pdPASS) ? 0 : 1;
    }
}


int osal_mutex_delete(osal_mutex_t mutex)
{
    assert(mutex);
    vSemaphoreDelete(mutex);
    return 0;
}
