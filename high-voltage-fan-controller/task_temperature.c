/*
 * task_temperature.c
 */ 

#include "task_temperature.h"
#include "task_uart.h"
#include "messages.h"
#include "printf_freertos.h"

// Internal task pid and static task resources
static xTaskHandle task_temperature_pid = NULL;

// Internal reference for the onewire device
static struct onewire_desc OW_1 = {
	pin: ONEWIRE
};

// Reference to sensor devices
static struct ds18b20_desc sensors[5];

static inline void scan_for_sensor()
{
	uart_write(MSG_SENSOR_SEARCH, MSG_SENSOR_SEARCH_LEN, 1000);

	onewire_search_t search;
	onewire_search_start(&OW_1, &search);
		
	uint8_t sensor_id = 0;
	onewire_addr_t addr;
	while (1)
	{
		if (sensor_id == 5)
		break;

		addr = onewire_search_next(&search);
		if (addr == ONEWIRE_NONE)
		{
			if (sensor_id < 5)
			for (uint8_t i = sensor_id; i < 5; i++)
				sensors[i].avail = false;
			break;
		}

		char buffer[18];
		int len = freertos_sprintf(
			&buffer[0],
			"found: 0x%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
			(uint8_t) (addr >> 56),
			(uint8_t) (addr >> 48),
			(uint8_t) (addr >> 40),
			(uint8_t) (addr >> 32),
			(uint8_t) (addr >> 24),
			(uint8_t) (addr >> 16),
			(uint8_t) (addr >>  8),
			(uint8_t) (addr >>  0)
		);
		uart_write(buffer, len, 1000);
		
		sensors[sensor_id].addr = addr;
		sensors[sensor_id].ow = &OW_1;
		sensors[sensor_id].avail = true;
		sensor_id++;
	}
}

static void temperature_task(void *pvParameters)
{
	(void) pvParameters;

	scan_for_sensor();
	vTaskDelay(ticks1s);

	while (1) 
	{
		uart_write("Reading sensors...\r\n", 20, 1000);
		for (uint8_t i = 0; i < 5; i++)
		{
			struct ds18b20_desc sensor = sensors[i];
			if (!sensor.avail) break;
			
			char buffer[18];
			int len = freertos_sprintf(
				&buffer[0],
				"sensor: 0x%02X%02X%02X%02X%02X%02X%02X%02X ",
				(uint8_t) (sensor.addr >> 56),
				(uint8_t) (sensor.addr >> 48),
				(uint8_t) (sensor.addr >> 40),
				(uint8_t) (sensor.addr >> 32),
				(uint8_t) (sensor.addr >> 24),
				(uint8_t) (sensor.addr >> 16),
				(uint8_t) (sensor.addr >>  8),
				(uint8_t) (sensor.addr >>  0)
			);
			uart_write(buffer, len, 1000);
			
			if (ds18b20_initiate_reading(&sensor))
			{
				vTaskDelay(ticks750ms);
				ds18b20_get_reading(&sensor);
				if (sensor.valid)
				{
					len = freertos_sprintf(&buffer[0], "=> %d", sensor.reading);
					uart_write(buffer, len, 1000);
				}
				uart_write("\r\n", 2, 1000);
			}
		
			vTaskDelay(ticks250ms);
		}
		vTaskDelay(ticks10s);
	}

	vTaskDelete(NULL);
}

struct ds18b20_desc *get_temperature_sensor(uint8_t idx)
{
	if (idx > 4) return NULL;
	if (!sensors[idx].avail) return NULL;
	return &sensors[idx];
}

BaseType_t create_temperature_task()
{
	if (task_temperature_pid != NULL) return pdFAIL;
	return xTaskCreate(temperature_task, "DS18B20", TASK_TEMPERATURE_STACK_SIZE, NULL, 2, task_temperature_pid);
	//return xTaskCreateStatic(temperature_task, "DS18B20", TASK_TEMPERATURE_STACK_SIZE, NULL, 2, xStack, &xTaskBuffer);
}

bool init_temperature_task()
{
	return true;
}

void kill_temperature_task()
{
	if (task_temperature_pid == NULL) return;
	vTaskDelete(task_temperature_pid);
	task_temperature_pid = NULL;
}
