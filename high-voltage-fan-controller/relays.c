
/*
 * relays.c
 */ 

#include <atmel_start.h>
#include "relays.h"
#include "task_uart.h"

inline bool relay_fan_channel_1_status()
{
	return gpio_get_pin_level(SWITCH_FAN_S1);
}

inline bool relay_fan_channel_2_status()
{
	return gpio_get_pin_level(SWITCH_FAN_S2);
}

inline bool relay_fan_channel_1_sensing()
{
	return !gpio_get_pin_level(SENSE_S1);
}

inline bool relay_fan_channel_2_sensing()
{
	return !gpio_get_pin_level(SENSE_S2);
}

inline bool relay_fan_channel_1_failure()
{
	return relay_fan_channel_1_status() != relay_fan_channel_1_sensing();
}

inline bool relay_fan_channel_2_failure()
{
	return relay_fan_channel_2_status() != relay_fan_channel_2_sensing();
}

void relay_fan_channel_1_enable(bool on)
{
	if (on && relay_fan_channel_2_status())
		relay_fan_channel_2_enable(false);

	gpio_set_pin_level(SWITCH_FAN_S1, on);

	if (on)
		uart_write("Fan: Channel 1 switched on\r\n", 28, 1000);
	else
		uart_write("Fan: Channel 1 switched off\r\n", 29, 1000);
}

void relay_fan_channel_2_enable(bool on)
{
	if (on && relay_fan_channel_1_status())
		relay_fan_channel_1_enable(false);

	gpio_set_pin_level(SWITCH_FAN_S2, on);

	if (on)
		uart_write("Fan: Channel 2 switched on\r\n", 28, 1000);
	else
		uart_write("Fan: Channel 2 switched off\r\n", 29, 1000);
}

void relay_fan_channels_off()
{
	if (relay_fan_channel_1_status())
		relay_fan_channel_1_enable(false);

	if (relay_fan_channel_2_status())
		relay_fan_channel_2_enable(false);
}

