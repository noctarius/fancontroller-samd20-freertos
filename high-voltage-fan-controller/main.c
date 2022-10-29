#include "main.h"

void exceptionHandler(uint32_t blinkMillis)
{
	kill_wdog_task();

	// Clear LED status
	gpio_set_pin_level(LED_1_R, true);
	gpio_set_pin_level(LED_1_G, true);
	gpio_set_pin_level(LED_1_B, true);
	gpio_set_pin_level(LED_2_R, true);
	gpio_set_pin_level(LED_2_G, true);
	gpio_set_pin_level(LED_2_B, true);

	while(1)
	{
		delay_ms(blinkMillis);
		gpio_toggle_pin_level(LED_1_R);
		gpio_toggle_pin_level(LED_2_R);
	}
}

int main(void)
{
	BaseType_t ret;

	// Initialize components configured through ASF
	atmel_start_init();

	// Switch off relays
	gpio_set_pin_level(SWITCH_FAN_S1, false);
	gpio_set_pin_level(SWITCH_FAN_S2, false);

	// Clear LED status
	gpio_set_pin_level(LED_1_R, true);
	gpio_set_pin_level(LED_1_G, true);
	gpio_set_pin_level(LED_1_B, true);
	gpio_set_pin_level(LED_2_R, true);
	gpio_set_pin_level(LED_2_G, true);
	gpio_set_pin_level(LED_2_B, true);

#if TASK_ENABLE_UART
	// Initialize uart
	init_uart_task();
	if ((ret = create_uart_task()) != pdPASS)
	{
		exceptionHandler(500);
	}
#endif

#if TASK_ENABLE_SELFTEST
	// Initialize selftest and keep alive
	init_selftest_task();
	if ((ret = create_selftest_task()) != pdPASS)
	{
		exceptionHandler(500);
	}
#endif

#if TASK_ENABLE_WATCHDOG
	// Initialize watchdog
	init_wdog_task();
	if ((ret = create_wdog_task()) != pdPASS)
	{
		exceptionHandler(500);
	}
#endif

#if TASK_ENABLE_DS18B20
	// Initialize ds18b20
	if (!init_temperature_task())
	{
		exceptionHandler(1000);
	}
	if ((ret = create_temperature_task()) != pdPASS)
	{
		exceptionHandler(500);
	}
#endif

#if TASK_ENABLE_I2C
	// Initialize i2c
	init_i2c_task();
	if ((ret = create_i2c_task()) != pdPASS)
	{
		exceptionHandler(500);
	}
#endif

#if TASK_ENABLE_AT24C256
	// Initialize at24c256
	init_at24_task();
	if ((ret = create_at24_task()) != pdPASS)
	{
		exceptionHandler(500);
	}
#endif

	// Start the FreeRTOS task scheduler
	vTaskStartScheduler();

	// Will never be reached
	while (1);
}

// FreeRTOS stack overflow handler
void vApplicationStackOverflowHook(TaskHandle_t *pxTask, signed char *pcTaskName)
{
	exceptionHandler(50);
}
// FreeRTOS malloc failed handler
void vApplicationMallocFailedHook(TaskHandle_t *pxTask, signed char *pcTaskName)
{
	exceptionHandler(200);
}

void vAssertCalled(char *file, uint32_t line)
{
	
}
