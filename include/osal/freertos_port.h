#pragma once

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "event_groups.h"

#include "hal/stm32f4xx.h"


// -- Thread
typedef void (*osal_thread_func_t)(void const *);
typedef TaskHandle_t osal_thread_t;

// -- Semaphore
typedef SemaphoreHandle_t osal_semaphore_t;


// -- Mutex
typedef SemaphoreHandle_t osal_mutex_t;


#define IN_ISR() (__get_IPSR() != 0)
