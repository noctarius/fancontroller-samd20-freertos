
/*
 * task_selftest.h
 */ 

#ifndef __TASK_SELFTEST_H__
#define __TASK_SELFTEST_H__

#include "atmel_start.h"
#include "FreeRTOS.h"
#include "ticks.h"
#include "main.h"
#include "subsystems.h"
#include "task_temperature.h"
#include "task_i2c.h"
#include "task_uart.h"
#include "task_eth.h"
#include "printf_freertos.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_SELFTEST_STACK_SIZE 128

BaseType_t create_selftest_task();

void init_selftest_task();

void kill_selftest_task();

#ifdef __cplusplus
}
#endif

#endif /* __TASK_SELFTEST_H__ */
