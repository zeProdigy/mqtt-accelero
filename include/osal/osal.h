#pragma once

#include <osal/freertos_port.h>


#define OSAL_MAX_TIMEOUT        (UINT32_MAX)
#define OSAL_DEFAULT_TIMEOUT    (1000)

typedef enum {
    THREAD_PRIORITY_IDLE,
    THREAD_PRIORITY_LOW,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_HIGH,
    THREAD_PRIORITY_REALTIME,
    THREAD_PRIORITY_ERROR,
} OSAL_THREAD_PRIORITY_T;


// -- Kernel
int osal_kernel_start();
int osal_kernel_systick_handler(void);
size_t osal_kernel_get_free_heap_size(void);


// -- Thread
osal_thread_t osal_thread_create(const char *name,
                               osal_thread_func_t func,
                               size_t stack_size,
                               OSAL_THREAD_PRIORITY_T prio,
                               void *params);
osal_thread_t osal_thread_self();
int osal_thread_delete(osal_thread_t osal_thread_t);
int osal_thread_stop(osal_thread_t thread);
int osal_thread_resume(osal_thread_t thread);
int osal_thread_stop_all(void);
void osal_thread_sleep(uint32_t ms);
void osal_thread_change_priority(osal_thread_t thread, OSAL_THREAD_PRIORITY_T priority);
void osal_thread_info_all(void *info);


// -- Semaphore
osal_semaphore_t osal_semaphore_create(void);
int osal_semaphore_release(osal_semaphore_t sem);
int osal_semaphore_wait(osal_semaphore_t sem, uint32_t ms_timeout);
int osal_semaphore_delete(osal_semaphore_t sem);


// -- Mutex
osal_mutex_t osal_mutex_create(void);
int osal_mutex_wait(osal_mutex_t mutex, uint32_t ms_timeout);
int osal_mutex_release(osal_mutex_t mutex);
int osal_mutex_delete(osal_mutex_t mutex);
