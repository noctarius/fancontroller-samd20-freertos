
/*
 * relays.h
 */ 

#ifndef __RELAYS_H__
#define __RELAYS_H__

#include <stdbool.h>

bool relay_fan_status_1();

bool relay_fan_status_2();

void relay_fan_speed_1(bool on);

void relay_fan_speed_2(bool on);

void relay_fan_speed_off();

#endif /* __RELAYS_H__ */
