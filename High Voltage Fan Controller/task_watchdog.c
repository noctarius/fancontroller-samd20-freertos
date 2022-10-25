/*
 * task_watchdog.c
 */ 

#include "task_watchdog.h"
#include "task_uart.h"
#include "messages.h"

// Internal task pid
static xTaskHandle task_watchdog_pid = NULL;

static void watchdog_task(void *pvParameters)
{
	write_uart(MSG_WATCHDOG_START, MSG_WATCHDOG_START_LEN, 1000);
	
	while (1)
	{	
		wdt_feed(&WDT_0);
		write_uart(MSG_WATCHDOG_FED, MSG_WATCHDOG_FED_LEN, 1000);
		vTaskDelay(ticks10s);
	}
}

BaseType_t create_wdog_task()
{
	if (task_watchdog_pid != NULL) return pdFAIL;
	return xTaskCreate(watchdog_task, "WDOG", TASK_WATCHDOG_STACK_SIZE, NULL, 2, task_watchdog_pid);
}

void init_wdog_task()
{
	wdt_set_timeout_period(&WDT_0, configTICK_RATE_HZ, 200000);
	wdt_feed(&WDT_0);
	wdt_enable(&WDT_0);
}

void kill_wdog_task()
{
	if (task_watchdog_pid == NULL) return;
	vTaskDelete(task_watchdog_pid);
	wdt_disable(&WDT_0);
	task_watchdog_pid = NULL;
}
