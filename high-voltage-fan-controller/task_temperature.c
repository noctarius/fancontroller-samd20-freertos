/*
 * task_temperature.c
 */ 

#include "task_temperature.h"
#include "task_uart.h"
#include "printf_freertos.h"
#include "eeprom.h"

// Internal task pid and static task resources
static xTaskHandle task_temperature_pid = NULL;

// Internal reference for the onewire device
static struct onewire_desc OW_1 = {
	pin: ONEWIRE
};

// Reference to sensor devices
static struct ds18b20_desc sensors[5];

static char printf_buf[64];

static inline void scan_for_sensor()
{
	uart_write("Sensor: Searching for sensors...\r\n", 34, 1000);

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

		int len = freertos_sprintf(
			printf_buf,
			"Sensor: Found 0x%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
			(uint8_t) (addr >> 56),
			(uint8_t) (addr >> 48),
			(uint8_t) (addr >> 40),
			(uint8_t) (addr >> 32),
			(uint8_t) (addr >> 24),
			(uint8_t) (addr >> 16),
			(uint8_t) (addr >>  8),
			(uint8_t) (addr >>  0)
		);
		uart_write(printf_buf, len, 1000);
		
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
		uart_write("Sensor: Updating temperatures...\r\n", 34, 1000);
		for (uint8_t i = 0; i < 5; i++)
		{
			struct ds18b20_desc *sensor = get_temperature_sensor_by_index(i);
			if (!sensor->avail) break;
			
			if (ds18b20_initiate_reading(sensor))
			{
				vTaskDelay(ticks750ms);
				
				int8_t sensor_id = eeprom_sensor_find_by_addr(sensor->addr);
				int16_t offset = eeprom_sensor_get_offset(sensor_id);
				
				ds18b20_get_reading(sensor, offset);
				if (sensor->valid)
				{
					int len = freertos_sprintf(
					printf_buf,
					"Sensor: 0x%02X%02X%02X%02X%02X%02X%02X%02X => %d\r\n",
					(uint8_t) (sensor->addr >> 56),
					(uint8_t) (sensor->addr >> 48),
					(uint8_t) (sensor->addr >> 40),
					(uint8_t) (sensor->addr >> 32),
					(uint8_t) (sensor->addr >> 24),
					(uint8_t) (sensor->addr >> 16),
					(uint8_t) (sensor->addr >>  8),
					(uint8_t) (sensor->addr >>  0),
					sensor->reading
					);
					uart_write(printf_buf, len, 1000);
				}
			}
		
			vTaskDelay(ticks250ms);
		}
		vTaskDelay(ticks10s);
	}

	vTaskDelete(NULL);
}

struct ds18b20_desc *get_temperature_sensor_by_index(uint8_t idx)
{
	if (idx > 4) return NULL;
	if (!sensors[idx].avail) return NULL;
	return &sensors[idx];
}

struct ds18b20_desc *get_temperature_sensor_by_addr(onewire_addr_t addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		if (sensors[i].avail && sensors[i].addr == addr)
			return &sensors[i];
	}
	return NULL;
}

uint16_t get_temperature_avg_outdoor()
{
	uint16_t sum = 0;
	uint16_t count = 0;
	uint8_t sensor_count = get_temperature_sensor_count();
	for (uint8_t i = 0; i < sensor_count; i++)
	{
		struct ds18b20_desc *sensor = get_temperature_sensor_by_index(i);
		if (!sensor->avail || !sensor->valid)
			continue;
			
		int8_t sensor_id = eeprom_sensor_find_by_addr(sensor->addr);
		if (!eeprom_sensor_get_indoor(sensor_id))
		{
			uint8_t weight = eeprom_sensor_get_weight(sensor_id);
			for (; weight > 0; weight--)
			{
				sum += sensor->reading;
				count++;
			}
		}
	}

	if (count == 0)
		return 0;

	return (sum / count);
}

uint16_t get_temperature_avg_indoor()
{
	uint16_t sum = 0;
	uint16_t count = 0;
	uint8_t sensor_count = get_temperature_sensor_count();
	for (uint8_t i = 0; i < sensor_count; i++)
	{
		struct ds18b20_desc *sensor = get_temperature_sensor_by_index(i);
		if (!sensor->avail || !sensor->valid)
			continue;
		
		int8_t sensor_id = eeprom_sensor_find_by_addr(sensor->addr);
		if (eeprom_sensor_get_indoor(sensor_id))
		{
			uint8_t weight = eeprom_sensor_get_weight(sensor_id);
			for (; weight > 0; weight--)
			{
				sum += sensor->reading;
				count++;
			}
		}
	}
	
	if (count == 0)
		return 0;
	
	return (sum / count);
}

uint8_t get_temperature_sensor_count()
{
	uint8_t count = 0;
	for (uint8_t i = 0; i < 5; i++)
	{
		if (sensors[i].avail) count++;
	}
	return count;
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
