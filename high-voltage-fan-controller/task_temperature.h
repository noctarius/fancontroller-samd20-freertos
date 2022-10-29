/*
 * task_temperature.h
 */ 

#ifndef __TASK_TEMPERATURE_H__
#define __TASK_TEMPERATURE_H__

#include "atmel_start.h"
#include "FreeRTOS.h"
#include "ds18b20.h"
#include "ticks.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_TEMPERATURE_STACK_SIZE configMINIMAL_STACK_SIZE

struct ds18b20_desc *get_temperature_sensor(uint8_t idx);

BaseType_t create_temperature_task();

bool init_temperature_task();

void kill_temperature_task();

#ifdef __cplusplus
}
#endif

#endif /* __TASK_TEMPERATURE_H__ */
