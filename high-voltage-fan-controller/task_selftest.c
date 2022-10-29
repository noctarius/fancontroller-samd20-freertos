
/*
 * task_selftest.c
 */ 

#include "task_selftest.h"
#include "task_uart.h"
#include "messages.h"

// Internal task pid
static xTaskHandle task_selftest_pid = NULL;

static void selftest_task(void *pvParameters)
{
	// Self test
	gpio_set_pin_level(LED_1_R, false);
	delay_ms(250);
	gpio_set_pin_level(LED_1_R, true);
	delay_ms(250);

	gpio_set_pin_level(LED_1_G, false);
	delay_ms(250);
	gpio_set_pin_level(LED_1_G, true);
	delay_ms(250);

	gpio_set_pin_level(LED_1_B, false);
	delay_ms(250);
	gpio_set_pin_level(LED_1_B, true);
	delay_ms(250);

	gpio_set_pin_level(LED_2_R, false);
	delay_ms(250);
	gpio_set_pin_level(LED_2_R, true);
	delay_ms(250);

	gpio_set_pin_level(LED_2_G, false);
	delay_ms(250);
	gpio_set_pin_level(LED_2_G, true);
	delay_ms(250);

	gpio_set_pin_level(LED_2_B, false);
	delay_ms(250);
	gpio_set_pin_level(LED_2_B, true);
	delay_ms(250);

	gpio_set_pin_level(SWITCH_FAN_S1, true);
	delay_ms(1000);
	gpio_set_pin_level(SWITCH_FAN_S1, false);
	delay_ms(1000);
	gpio_set_pin_level(SWITCH_FAN_S2, true);
	delay_ms(1000);
	gpio_set_pin_level(SWITCH_FAN_S2, false);

	while (1)
	{
		vTaskDelay(ticks1s);
		gpio_set_pin_level(LED_2_G, false);
		vTaskDelay(ticks250ms);
		gpio_set_pin_level(LED_2_G, true);
	}
}

BaseType_t create_selftest_task()
{
	if (task_selftest_pid != NULL) return pdFAIL;
	return xTaskCreate(selftest_task, "SELFTEST", TASK_SELFTEST_STACK_SIZE, NULL, 2, task_selftest_pid);	
}

void init_selftest_task()
{
}

void kill_selftest_task()
{
	if (task_selftest_pid == NULL) return;
	vTaskDelete(task_selftest_pid);
	task_selftest_pid = NULL;
}
