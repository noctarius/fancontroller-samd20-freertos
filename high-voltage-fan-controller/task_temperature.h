/*
 * task_temperature.h
 */ 

#ifndef __TASK_TEMPERATURE_H__
#define __TASK_TEMPERATURE_H__

#include "atmel_start.h"
#include "FreeRTOS.h"
#include "ds18b20.h"
#include "ticks.h"
#include "onewire.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_TEMPERATURE_STACK_SIZE 120

struct ds18b20_desc *get_temperature_sensor_by_index(uint8_t idx);

struct ds18b20_desc *get_temperature_sensor_by_addr(onewire_addr_t addr);

uint8_t get_temperature_sensor_count();

uint16_t get_temperature_avg_outdoor();

uint16_t get_temperature_avg_indoor();

BaseType_t create_temperature_task();

bool init_temperature_task();

void kill_temperature_task();

#ifdef __cplusplus
}
#endif

#endif /* __TASK_TEMPERATURE_H__ */
