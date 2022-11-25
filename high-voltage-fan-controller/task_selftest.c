
/*
 * task_selftest.c
 */ 

#include "task_selftest.h"
#include "at24c256.h"
#include "eeprom.h"
#include "relays.h"
#include "leds.h"
#include "timer.h"

// Internal task pid
static xTaskHandle task_selftest_pid = NULL;

static Timer timer_threshold;

static bool runHealthChecks()
{
	uint8_t sensor_count = get_temperature_sensor_count();
	if (sensor_count == 0)
	{
		return false;
	}
	
	for (uint8_t i = 0; i < sensor_count; i++)
	{
		struct ds18b20_desc *sensor = get_temperature_sensor_by_index(i);
		if (!sensor->valid)
			return false;
	}
	
	if (exceeded_expected_update_rate())
	{
		return false;
	}
	
	return true;
}

static void selftest_task(void *pvParameters)
{
	#if TASK_ENABLE_WATCHDOG
		wdt_set_timeout_period(&WDT_0, 1000, 16384);
		wdt_feed(&WDT_0);
		wdt_enable(&WDT_0);
		uart_write("Watchdog: Started.\r\n", 20, 1000);
	#endif
	
	// Self test
	uart_write("Selftest: Running...\r\n", 22, 1000);
	
	led1(true, false, false);
	delay_ms(250);
	led1_off();
	delay_ms(250);

	led1(false, true, false);
	delay_ms(250);
	led1_off();
	delay_ms(250);

	led1(false, false, true);
	delay_ms(250);
	led1_off();
	delay_ms(250);

	#if TASK_ENABLE_WATCHDOG
		wdt_feed(&WDT_0);
	#endif

	led2(true, false, false);
	delay_ms(250);
	led2_off();
	delay_ms(250);

	led2(false, true, false);
	delay_ms(250);
	led2_off();
	delay_ms(250);

	led2(false, false, true);
	delay_ms(250);
	led2_off();
	delay_ms(250);

	#if TASK_ENABLE_WATCHDOG
		wdt_feed(&WDT_0);
	#endif

	relay_fan_speed_1(true);
	delay_ms(1000);
	relay_fan_speed_off();

	#if TASK_ENABLE_WATCHDOG
		wdt_feed(&WDT_0);
	#endif

	delay_ms(1000);

	#if TASK_ENABLE_WATCHDOG
		wdt_feed(&WDT_0);
	#endif

	relay_fan_speed_2(true);
	delay_ms(1000);
	relay_fan_speed_off();

	#if TASK_ENABLE_WATCHDOG
		wdt_feed(&WDT_0);
	#endif
	
	uart_write("Selftest: Passed.\r\n", 19, 1000);

	// Start other subsystems (if enabled)
	BaseType_t ret;
	#if TASK_ENABLE_I2C	
		// Initialize i2c
		uart_write("I2C: Initializing...\r\n", 22, 1000);
		init_i2c_task();
		if ((ret = create_i2c_task()) != pdPASS)
		{
			exceptionHandler(500);
		}
		uart_write("I2C: Initialized.\r\n", 19, 1000);
	#endif

	#if TASK_ENABLE_DS18B20
		// Initialize ds18b20
		uart_write("Sensor: Initializing...\r\n", 25, 1000);
		if (!init_temperature_task())
		{
			exceptionHandler(1000);
		}
		if ((ret = create_temperature_task()) != pdPASS)
		{
			exceptionHandler(500);
		}
		uart_write("Sensor: Initialized.\r\n", 22, 1000);
	#endif

	#if TASK_ENABLE_MQTT
		// Initialize ETH / MQTT
		uart_write("MQTT: Initializing...\r\n", 23, 1000);
		init_eth_task();
		if ((ret = create_eth_task()) != pdPASS)
		{
			exceptionHandler(500);
		}
		uart_write("MQTT: Initialized.\r\n", 20, 1000);
	#endif

	#if TASK_ENABLE_WATCHDOG
		wdt_feed(&WDT_0);
	#endif

	// Give the subsystems time to start up
	for (uint8_t i = 0; i < 10; i++)
	{
		vTaskDelay(ticks1s);
		#if TASK_ENABLE_WATCHDOG
			wdt_feed(&WDT_0);
		#endif
	}

	TimerInit(&timer_threshold);
	while (1)
	{
		vTaskDelay(ticks1s);
		led2_g(true);
		vTaskDelay(ticks250ms);
		led2_g(false);
		
		#if TASK_ENABLE_WATCHDOG
			// Feed watchdog
			if (runHealthChecks())
			{
				wdt_feed(&WDT_0);
			}
		#endif
		
		#if TASK_ENABLE_MONITOR
			if (TimerIsExpired(&timer_threshold))
			{
				uint16_t outdoor = get_temperature_avg_outdoor();
				uint16_t indoor = get_temperature_avg_indoor();

				if (indoor > 3000)
				{
					relay_fan_speed_2(true);
				}
			
				if (indoor > 2500 && outdoor < 2000)
				{
					relay_fan_speed_2(true);
				}
			
				if (indoor < 1800)
				{
					relay_fan_speed_off();
				}
			
				for (uint8_t i = 0; i < 5; i++)
				{
					struct ds18b20_desc *sensor = get_temperature_sensor_by_index(i);
					if (sensor == NULL)
						continue;
					
					if (!sensor->valid || !sensor->avail)
						continue;
				
					int8_t sensor_id = eeprom_sensor_find_by_addr(sensor->addr);
					if (sensor_id == -1)
						continue;
				
					if (!eeprom_sensor_get_indoor(sensor_id))
						continue;
				
					uint16_t threshold = eeprom_sensor_get_security_threshold(sensor_id);
					if (threshold > 0 && threshold <= sensor->reading)
					{
						relay_fan_speed_2(true);
						break;
					}
				}
				
				TimerCountdownMS(&timer_threshold, 10000);
			}
		#endif
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

	#if TASK_ENABLE_WATCHDOG
		wdt_disable(&WDT_0);
	#endif
	
	vTaskDelete(task_selftest_pid);
	task_selftest_pid = NULL;
}
