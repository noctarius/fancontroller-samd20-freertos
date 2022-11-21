
/*
 * eeprom.h
 */ 

#ifndef __EEPROM_H__
#define __EEPROM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "onewire.h"

#define DISABLED_SENSOR_THRESHOLD ((uint16_t) 1) >> 1

void eeprom_initialize_storage();

int8_t eeprom_sensor_find_by_addr(const onewire_addr_t addr);

int8_t eeprom_sensor_find_first_unused_index();

bool eeprom_sensor_assignment_slot(uint8_t sensor_id);

void eeprom_sensor_unassignment_slot(uint8_t sensor_id);

void eeprom_sensor_set_name(const uint8_t sensor_id, const char *name, const uint8_t name_len);

void eeprom_sensor_set_addr(const uint8_t sensor_id, const onewire_addr_t addr);

void eeprom_sensor_set_security_threshold(const uint8_t sensor_id, const uint16_t threshold);

void eeprom_sensor_set_offset(const uint8_t sensor_id, const int16_t offset);

void eeprom_sensor_set_indoor(const uint8_t sensor_id, const bool indoor);

uint8_t eeprom_sensor_get_name(const int8_t sensor_id, const char *name);

onewire_addr_t eeprom_sensor_get_addr(const int8_t sensor_id);

uint16_t eeprom_sensor_get_security_threshold(const int8_t sensor_id);

int16_t eeprom_sensor_get_offset(const int8_t sensor_id);

bool eeprom_sensor_get_indoor(const int8_t sensor_id);

void eeprom_set_mac_addr(uint8_t mac_addr[6]);

void eeprom_get_mac_addr(const uint8_t mac_addr[6]);

void eeprom_mqtt_set_addr(const uint8_t addr[4]);

void eeprom_mqtt_set_port(const uint16_t port);

void eeprom_mqtt_get_addr(const uint8_t addr[4]);

uint16_t eeprom_mqtt_get_port();

#ifdef __cplusplus
}
#endif

#endif
