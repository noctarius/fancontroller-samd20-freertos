/*
 * main.h
 */ 

#ifndef __MAIN_H__
#define __MAIN_H__

#include <atmel_start.h>
#include <FreeRTOS.h>
#include <task.h>
#include "subsystems.h"
#include "task_uart.h"
#include "task_watchdog.h"
#include "task_selftest.h"
#include "task_eth.h"
#include "at24c256.h"

#ifdef __cplusplus
extern "C" {
#endif

void exceptionHandler(uint32_t blinkMillis);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */