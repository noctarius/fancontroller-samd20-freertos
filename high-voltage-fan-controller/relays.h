
/*
 * relays.h
 */ 

#ifndef __RELAYS_H__
#define __RELAYS_H__

#include <stdbool.h>

bool relay_fan_channel_1_status();

bool relay_fan_channel_2_status();

bool relay_fan_channel_1_sensing();

bool relay_fan_channel_2_sensing();

bool relay_fan_channel_1_failure();

bool relay_fan_channel_2_failure();

void relay_fan_channel_1_enable(bool on);

void relay_fan_channel_2_enable(bool on);

void relay_fan_channels_off();

#endif /* __RELAYS_H__ */
