/*
 * task_temperature.c
 */ 

#include "task_temperature.h"
#include "task_uart.h"
#include "messages.h"
#include "printf_freertos.h"

// Internal task pid
static xTaskHandle task_temperature_pid = NULL;

static struct onewire_desc OW_1 = {
	pin: ONEWIRE
};

static struct ds18b20_desc *sensors[5];

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
		char buffer[18];
		int len = freertos_sprintf(&buffer[0], "%d\r\n", addr);
		uart_write(buffer, len, 1000);

		if (addr == ONEWIRE_NONE)
		{
			if (sensor_id < 5)
			for (uint8_t i = sensor_id; i < 5; i++)
			sensors[i] = NULL;
			break;
		}
		
		struct ds18b20_desc sensor = {
			addr: addr,
			ow: &OW_1,
		};
			
		sensors[sensor_id] = &sensor;
			
		sensor_id++;
	}
}

static void temperature_task(void *pvParameters)
{
	(void) pvParameters;

	while (1) 
	{
		for (uint8_t i = 0; i < 5; i++)
		{
			struct ds18b20_desc *sensor = sensors[i];
			if (sensor == NULL) break;
			
			if (ds18b20_initiate_reading(sensor))
			{
				vTaskDelay(ticks750ms);
				ds18b20_get_reading(sensor);				
			}
		
			vTaskDelay(ticks250ms);
		}
		vTaskDelay(ticks10s);
		scan_for_sensor();
	}

	vTaskDelete(NULL);
}

struct ds18b20_desc *get_temperature_sensor(uint8_t idx)
{
	if (idx > 4) return NULL;
	return sensors[idx];
}

BaseType_t create_temperature_task()
{
	if (task_temperature_pid != NULL) return pdFAIL;
	return xTaskCreate(temperature_task, "DS18B20", TASK_TEMPERATURE_STACK_SIZE, NULL, 2, task_temperature_pid);
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
