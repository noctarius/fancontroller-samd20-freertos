
/*
 * eeprom.c
 */ 

#include "eeprom.h"
#include "at24c256.h"
#include "FreeRTOS.h"
#include "task_uart.h"
#include <string.h>

typedef struct __attribute__((__packed__))
{
	onewire_addr_t	addr;
	char			name[100];
	uint8_t			weight;
	uint8_t			reserved[155];
	uint8_t			name_len;
	uint16_t		sec_threshold;
	int16_t			offset;
	struct __attribute__((__packed__))
	{
		bool	indoor:		1;
		bool	assigned:	1;
		uint8_t	reserved:	6;
	} flags;
} sensor_desc;

typedef struct __attribute__((__packed__))
{
	uint8_t		addr[4];
	uint16_t	port;
	uint8_t		test;
} mqtt_server_desc;

typedef struct __attribute__((__packed__))
{
	uint8_t				header[4];
	uint8_t				mac_addr[6];
	mqtt_server_desc	mqtt_server;
	sensor_desc			sensors[5];
} eeprom_desc;

static eeprom_desc eeprom_info;
static uint16_t eemprom_desc_len = sizeof(eeprom_desc);
static volatile bool eeprom_init = false;
static volatile bool eeprom_init_in_progress = false;

static struct at24c256_desc AT24_0 =
{
	addr: 0x50
};

static void ensure_initialized()
{
	if (eeprom_init) return;
	
	if (eeprom_init_in_progress)
	{
		while (eeprom_init_in_progress)
		{
			vTaskDelay(ticks100ms);
		}
		return;
	}
	
	eeprom_init_in_progress = true;
	
	TaskHandle_t handle = xTaskGetCurrentTaskHandle();
	if (handle == NULL) return;
	
    uart_write("EEPROM: Initializing...\r\n", 25, 1000);
	at24_read(&AT24_0, 0, (uint8_t *) &eeprom_info, eemprom_desc_len, 10000);

	if (eeprom_info.header[0] != 0xAA
		|| eeprom_info.header[1] != 0xBB
		|| eeprom_info.header[2] != 0xCC
		|| eeprom_info.header[3] != 0xDD
		|| eeprom_info.mqtt_server.test != 1)
	{
		eeprom_initialize_storage();
	}
	
	eeprom_init = true;
	eeprom_init_in_progress = false;
    uart_write("EEPROM: Initialized.\r\n", 22, 1000);
}

static void store_eeprom_content()
{
	at24_write(&AT24_0, 0, (uint8_t *) &eeprom_info, eemprom_desc_len, 10000);
}

void eeprom_initialize_storage()
{
	eeprom_init_in_progress = true;
	eeprom_init = false;
    uart_write("EEPROM: Clearing...\r\n", 21, 1000);
	memset(&eeprom_info, 0, eemprom_desc_len);
	eeprom_info.header[0] = 0xAA;
	eeprom_info.header[1] = 0xBB;
	eeprom_info.header[2] = 0xCC;
	eeprom_info.header[3] = 0xDD;
	eeprom_info.mqtt_server.test = 1;
	eeprom_info.mac_addr[0] = 0x7A;
	eeprom_info.mac_addr[1] = 0xD3;
	eeprom_info.mac_addr[2] = 0xF0;
	eeprom_info.mac_addr[3] = 0x7C;
	eeprom_info.mac_addr[4] = 0x0C;
	eeprom_info.mac_addr[5] = 0x2E;
	eeprom_info.mqtt_server.addr[0] = 10;
	eeprom_info.mqtt_server.addr[1] = 96;
	eeprom_info.mqtt_server.addr[2] = 0;
	eeprom_info.mqtt_server.addr[3] = 71;
	eeprom_info.mqtt_server.port = 1883;
	store_eeprom_content();
    uart_write("EEPROM: Cleared.\r\n", 18, 1000);
	uart_write("EEPROM: Reinitializing...\r\n", 27, 1000);
	at24_read(&AT24_0, 0, (uint8_t *) &eeprom_info, eemprom_desc_len, 10000);
	uart_write("EEPROM: Reinitialized.\r\n", 24, 1000);
	eeprom_init = true;
	eeprom_init_in_progress = false;
}

int8_t eeprom_sensor_find_by_addr(const onewire_addr_t addr)
{
	ensure_initialized();
	for (uint8_t i = 0; i < 5; i++)
	{
		if (!eeprom_info.sensors[i].flags.assigned)
			continue;

		if (eeprom_info.sensors[i].addr == addr)
			return (int8_t) i;
	}
	return -1;
}

int8_t eeprom_sensor_find_first_unused_index()
{
	ensure_initialized();
	for (uint8_t i = 0; i < 5; i++)
	{
		if (!eeprom_info.sensors[i].flags.assigned)
			return (int8_t) i;
	}
	return -1;
}

bool eeprom_sensor_assignment_slot(uint8_t sensor_id)
{
	if (sensor_id == -1)
		return false;
	
	ensure_initialized();
	if (eeprom_info.sensors[sensor_id].flags.assigned)
		return false;

	eeprom_info.sensors[sensor_id].flags.assigned = true;
	store_eeprom_content();
	return true;
}

void eeprom_sensor_unassignment_slot(uint8_t sensor_id)
{
	if (sensor_id == -1)
		return;
	
	ensure_initialized();
	eeprom_info.sensors[sensor_id].flags.assigned = false;
	store_eeprom_content();
}

void eeprom_sensor_set_weight(const uint8_t sensor_id, const uint8_t weight)
{
	if (sensor_id == -1)
	return;
	
	ensure_initialized();
	eeprom_info.sensors[sensor_id].weight = weight;
	store_eeprom_content();
}

void eeprom_sensor_set_name(const uint8_t sensor_id, const char *name, const uint8_t name_len)
{
	if (sensor_id == -1)
		return;
	
	if (name_len > 100)
		return;

	ensure_initialized();
	if (!eeprom_info.sensors[sensor_id].flags.assigned)
		return;

	memcpy(&eeprom_info.sensors[sensor_id].name, name, name_len);
	eeprom_info.sensors[sensor_id].name_len = name_len;
	store_eeprom_content();
}

void eeprom_sensor_set_addr(const uint8_t sensor_id, const onewire_addr_t addr)
{
	if (sensor_id == -1)
		return;
	
	ensure_initialized();
	if (!eeprom_info.sensors[sensor_id].flags.assigned)
		return;

	eeprom_info.sensors[sensor_id].addr = addr;
	store_eeprom_content();
}

void eeprom_sensor_set_security_threshold(const uint8_t sensor_id, const uint16_t threshold)
{
	if (sensor_id == -1)
		return;
	
	ensure_initialized();
	if (!eeprom_info.sensors[sensor_id].flags.assigned)
		return;

	eeprom_info.sensors[sensor_id].sec_threshold = threshold;
	store_eeprom_content();
}

void eeprom_sensor_set_offset(const uint8_t sensor_id, const int16_t offset)
{
	if (sensor_id == -1)
		return;
	
	ensure_initialized();
	if (!eeprom_info.sensors[sensor_id].flags.assigned)
		return;

	eeprom_info.sensors[sensor_id].offset = offset;
	store_eeprom_content();
}

void eeprom_sensor_set_indoor(const uint8_t sensor_id, const bool indoor)
{
	if (sensor_id == -1)
		return;
	
	ensure_initialized();
	if (!eeprom_info.sensors[sensor_id].flags.assigned)
		return;

	eeprom_info.sensors[sensor_id].flags.indoor = indoor ? 1 : 0;
	store_eeprom_content();
}

uint8_t eeprom_sensor_get_weight(const int8_t sensor_id)
{
	if (sensor_id == -1)
		return 1;
	
	ensure_initialized();
	if (!eeprom_info.sensors[sensor_id].flags.assigned)
		return 1;

	uint8_t weight = eeprom_info.sensors[sensor_id].weight;
	return weight > 0 ? weight : 1;
}

uint8_t eeprom_sensor_get_name(const int8_t sensor_id, const char *name)
{
	if (sensor_id == -1)
		return 0;
	
	ensure_initialized();
	if (!eeprom_info.sensors[sensor_id].flags.assigned)
		return 0;

	memcpy((char *)name, eeprom_info.sensors[sensor_id].name, eeprom_info.sensors[sensor_id].name_len);
	return eeprom_info.sensors[sensor_id].name_len;
}

onewire_addr_t eeprom_sensor_get_addr(const int8_t sensor_id)
{
	if (sensor_id == -1)
		return 0;
	
	ensure_initialized();
	if (!eeprom_info.sensors[sensor_id].flags.assigned)
		return 0;

	return eeprom_info.sensors[sensor_id].addr;
}

uint16_t eeprom_sensor_get_security_threshold(const int8_t sensor_id)
{
	if (sensor_id == -1)
		return 0;
	
	ensure_initialized();
	if (!eeprom_info.sensors[sensor_id].flags.assigned)
		return 0;

	return eeprom_info.sensors[sensor_id].sec_threshold;
}

int16_t eeprom_sensor_get_offset(const int8_t sensor_id)
{
	if (sensor_id == -1)
		return 0;
	
	ensure_initialized();
	if (!eeprom_info.sensors[sensor_id].flags.assigned)
		return 0;

	return eeprom_info.sensors[sensor_id].offset;
}

bool eeprom_sensor_get_indoor(const int8_t sensor_id)
{
	if (sensor_id == -1)
		return false;
	
	ensure_initialized();
	if (!eeprom_info.sensors[sensor_id].flags.assigned)
		return false;

	return eeprom_info.sensors[sensor_id].flags.indoor == 1 ? true : false;
}

void eeprom_set_mac_addr(uint8_t mac_addr[6])
{
	ensure_initialized();
	memcpy(eeprom_info.mac_addr, mac_addr, 6);
	store_eeprom_content();
}

void eeprom_get_mac_addr(const uint8_t mac_addr[6])
{
	ensure_initialized();
	memcpy((void *) mac_addr, eeprom_info.mac_addr, 6);
	
	if (mac_addr[0] == 0x00
		&& mac_addr[1] == 0x00
		&& mac_addr[2] == 0x00
		&& mac_addr[3] == 0x00
		&& mac_addr[4] == 0x00
		&& mac_addr[5] == 0x00
	)
	{
		*(((uint8_t *) mac_addr) + 0) = 0x7A;
		*(((uint8_t *) mac_addr) + 1) = 0xD3;
		*(((uint8_t *) mac_addr) + 2) = 0xF0;
		*(((uint8_t *) mac_addr) + 3) = 0x7C;
		*(((uint8_t *) mac_addr) + 4) = 0x0C;
		*(((uint8_t *) mac_addr) + 5)= 0x2E;
	}
}

void eeprom_mqtt_set_addr(const uint8_t addr[4])
{
	ensure_initialized();
	memcpy(eeprom_info.mqtt_server.addr, addr, 4);
	store_eeprom_content();
}

void eeprom_mqtt_set_port(const uint16_t port)
{
	ensure_initialized();
	eeprom_info.mqtt_server.port = port;
	store_eeprom_content();
}

void eeprom_mqtt_get_addr(const uint8_t addr[4])
{
	ensure_initialized();
	memcpy((void *) addr, eeprom_info.mqtt_server.addr, 4);
	if (addr[0] == 0x00
		&& addr[1] == 0x00
		&& addr[2] == 0x00 
		&& addr[3] == 0x00
	)
	{
		*(((uint8_t *) addr) + 0) = 10;
		*(((uint8_t *) addr) + 1) = 96;
		*(((uint8_t *) addr) + 2) = 0;
		*(((uint8_t *) addr) + 3) = 71;
	}
}

uint16_t eeprom_mqtt_get_port()
{
	ensure_initialized();
	if (eeprom_info.mqtt_server.port > 0)
		return eeprom_info.mqtt_server.port;
		
	return 1883;
}
