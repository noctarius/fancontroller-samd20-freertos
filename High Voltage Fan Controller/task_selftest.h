
/*
 * task_selftest.h
 */ 

#ifndef __TASK_SELFTEST_H__
#define __TASK_SELFTEST_H__

#include "atmel_start.h"
#include "FreeRTOS.h"
#include "ticks.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_SELFTEST_STACK_SIZE configMINIMAL_STACK_SIZE

BaseType_t create_selftest_task();

void init_selftest_task();

void kill_selftest_task();

#ifdef __cplusplus
}
#endif

#endif /* __TASK_SELFTEST_H__ */
