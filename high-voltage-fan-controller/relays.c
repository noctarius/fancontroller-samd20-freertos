
/*
 * relays.c
 */ 

#include <atmel_start.h>
#include "relays.h"
#include "task_uart.h"

static bool status_fan_speed_1 = false;
static bool status_fan_speed_2 = false;

void relay_fan_speed_1(bool on)
{
	if (on && status_fan_speed_2)
		relay_fan_speed_2(false);

	gpio_set_pin_level(SWITCH_FAN_S1, on);
	status_fan_speed_1 = on;
	if (on)
		uart_write("Fan: Speed 1 switched on\r\n", 26, 1000);
	else
		uart_write("Fan: Speed 1 switched off\r\n", 27, 1000);
}

void relay_fan_speed_2(bool on)
{
	if (on && status_fan_speed_1)
		relay_fan_speed_1(false);

	gpio_set_pin_level(SWITCH_FAN_S2, on);
	status_fan_speed_2 = on;
	if (on)
		uart_write("Fan: Speed 2 switched on\r\n", 26, 1000);
	else
		uart_write("Fan: Speed 2 switched off\r\n", 27, 1000);
}

void relay_fan_speed_off()
{
	relay_fan_speed_1(false);
	relay_fan_speed_2(false);
}

