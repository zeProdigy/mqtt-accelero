#include <assert.h>
#include "osal/osal.h"


static const UBaseType_t to_freertos_priority(OSAL_THREAD_PRIORITY_T priority)
{
    return tskIDLE_PRIORITY + priority;
}


osal_thread_t osal_thread_self(void)
{
    return (osal_thread_t)xTaskGetCurrentTaskHandle();
}


osal_thread_t osal_thread_create(const char *name,
                                 osal_thread_func_t func,
                                 size_t stack_size,
                                 OSAL_THREAD_PRIORITY_T prio,
                                 void *params)
{
    assert(name);
    assert(func);

    TaskHandle_t xHandle = pvPortMalloc(sizeof(xHandle));

    BaseType_t ret = xTaskCreate((TaskFunction_t)func,
                                 name,
                                 stack_size,
                                 params,
                                 to_freertos_priority(prio),
                                 xHandle);

    return (ret == pdPASS) ? xHandle : NULL;
}


int osal_thread_delete(osal_thread_t thread)
{
    assert(thread);
    vTaskDelete(thread);
    return 0;
}


int osal_thread_stop(osal_thread_t thread)
{
    assert(thread);
    vTaskSuspend(thread);
    return 0;
}


int osal_thread_stop_all(void)
{
    vTaskSuspendAll();
    return 0;
}


int osal_thread_resume(osal_thread_t thread)
{
    assert(thread);

    if (IN_ISR()) {
        BaseType_t xHigherPriorityTaskWoken;
        xHigherPriorityTaskWoken = xTaskResumeFromISR(thread);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        vTaskResume(thread);
    }

    return 0;
}


void osal_thread_sleep(uint32_t ms)
{
    TickType_t ticks = ms / portTICK_PERIOD_MS;
    vTaskDelay(ticks ? ticks : 1);
}


void osal_thread_change_priority(osal_thread_t thread, OSAL_THREAD_PRIORITY_T priority)
{
    vTaskPrioritySet(thread, to_freertos_priority(priority));
}


void osal_thread_info_all(void *info)
{
    assert(info);
    vTaskList((char *)info);
}

