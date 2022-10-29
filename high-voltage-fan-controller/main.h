/*
 * main.h
 */ 

#ifndef __MAIN_H__
#define __MAIN_H__

#include <atmel_start.h>
#include <FreeRTOS.h>
#include <task.h>
#include "task_temperature.h"
#include "task_uart.h"
#include "task_i2c.h"
#include "task_watchdog.h"
#include "task_selftest.h"

// Enabled subsystems
#define TASK_ENABLE_UART		1
#define TASK_ENABLE_DS18B20		1
#define TASK_ENABLE_I2C			1
#define TASK_ENABLE_AT24C256	0
#define TASK_ENABLE_WATCHDOG	0
#define TASK_ENABLE_MONITOR		0
#define TASK_ENABLE_SWITCH		0
#define TASK_ENABLE_MQTT		0
#define TASK_ENABLE_SELFTEST	1

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */