/*
 * task_watchdog.h
 */ 

#ifndef __TASK_WATCHDOG_H__
#define __TASK_WATCHDOG_H__

#include "atmel_start.h"
#include "ticks.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_WATCHDOG_STACK_SIZE configMINIMAL_STACK_SIZE

BaseType_t create_wdog_task();

void init_wdog_task();

void kill_wdog_task();

#ifdef __cplusplus
}
#endif

#endif /* __TASK_WATCHDOG_H__ */