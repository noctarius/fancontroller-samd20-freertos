
/*
 * task_selftest.c
 */ 

#include "task_selftest.h"
#include "messages.h"
#include "at24c256.h"
#include "eeprom.h"

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

	// Start other subsystems (if enabled)
	BaseType_t ret;
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

	// Give the subsystems time to start up
	vTaskDelay(ticks1s);
	
	// Test write EEPROM
    uart_write("Initialize EEPROM...\r\n", 22, 1000);
	struct at24c256_desc AT24_0 =
	{
		addr: 0x50
	};
	
	uint16_t eeprom_len = sizeof(eeprom_desc);
	at24_read(&AT24_0, 0, (uint8_t *) &eeprom_desc, eeprom_len, 1000);
	char buffer[18];
	int len = freertos_sprintf(
		&buffer[0],
		"at24: 0x%02X%02X%02X%02X\r\n",
		eeprom_desc.header[0],
		eeprom_desc.header[1],
		eeprom_desc.header[2],
		eeprom_desc.header[3]
	);
	uart_write(buffer, len, 1000);

	vTaskDelay(ticks250ms);
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
