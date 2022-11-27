
/*
 * task_eth.c
 */ 

#include "task_eth.h"
#include "dhcp.h"
#include "mqtt_interface.h"
#include "MQTTClient.h"
#include "task_temperature.h"
#include "ds18b20.h"
#include "cbor.h"
#include "eeprom.h"
#include "relays.h"

// Internal task pid
static xTaskHandle task_eth_pid = NULL;
static char buf[256];
static unsigned char mqtt_send_buf[1280];
static unsigned char mqtt_recv_buf[256];
static uint8_t dhcp_buf[548];
static uint8_t mqtt_buf[1024];

static SemaphoreHandle_t sem_eth_isr;
static wiz_NetInfo net_info;
static Network network;
static MQTTClient client;
static MQTTString client_id = {
	cstring: "fancontrol\0"
};
static MQTTMessage msg;

static uint8_t target_ip[4];
static uint16_t target_port;

static char temp_subtopic[50];

static Timer timer_last_update;
static Timer timer_update;

static inline uint8_t dehex(char hex)
{
	if (hex >= 0x41)
	{
		// case insensitive
		hex &= 0b01011111;
		// ASCII A -> 65
		hex -= 55;
	}
	else
	{
		// ASCII 0 -> 48
		hex -= 48;
	}
	return (uint8_t) hex;
}

static bool valid_ip_addr(wiz_NetInfo *netInfo)
{
	if (netInfo == NULL) return false;
	return netInfo->ip[0] != 0x00 && netInfo->ip[1] != 0x00 && netInfo->ip[2] != 0x00 && netInfo->ip[3] != 0x00;
}

static void send_mqtt_settings_update(MQTTClient *client)
{
	uart_write("MQTT: Send Settings Update\r\n", 28, 1000);

	CborEncoder encoder;
	cbor_encoder_init(&encoder, mqtt_buf, sizeof(mqtt_buf), 0);

	CborEncoder map1;
	CborEncoder map2;

	cbor_encoder_create_map(&encoder, &map1, 2);
	
	cbor_encode_text_string(&map1, "main", 4);
	cbor_encoder_create_map(&map1, &map2, 1);
	
	cbor_encode_text_string(&map2, "macaddr", 7);
	uint8_t tmp[6];
	eeprom_get_mac_addr(tmp);
	int len = freertos_sprintf(
		buf, "%02X:%02X:%02X:%02X:%02X:%02X", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5]
	);
	cbor_encode_text_string(&map2, buf, len);
	cbor_encoder_close_container(&map1, &map2);
	
	cbor_encode_text_string(&map1, "mqtt", 4);
	cbor_encoder_create_map(&map1, &map2, 2);
	
	cbor_encode_text_string(&map2, "host", 4);
	eeprom_mqtt_get_addr(tmp);
	len = freertos_sprintf(
		buf, "%d.%d.%d.%d", tmp[0], tmp[1], tmp[2], tmp[3]
	);
	cbor_encode_text_string(&map2, buf, len);
	
	cbor_encode_text_string(&map2, "port", 4);
	uint16_t port = eeprom_mqtt_get_port();
	cbor_encode_uint(&map2, port);
	
	cbor_encoder_close_container(&map1, &map2);
	cbor_encoder_close_container(&encoder, &map1);

	msg.qos = QOS0;
	msg.retained = 1;
	msg.payload = mqtt_buf;
	msg.payloadlen = cbor_encoder_get_buffer_size(&encoder, mqtt_buf);
	MQTTPublish(client, "fancontrol/configuration\0", &msg);
}

static void send_mqtt_status_update(MQTTClient *client)
{
	uart_write("MQTT: Send Status Update\r\n", 26, 1000);

	CborEncoder encoder;
	cbor_encoder_init(&encoder, mqtt_buf, sizeof(mqtt_buf), 0);
	
	CborEncoder items;
	cbor_encoder_create_map(&encoder, &items, 6);
	
	// uptime
	uint64_t secs = xTickToMs() / 1000;
	cbor_encode_text_string(&items, "uptime", 6);
	cbor_encode_uint(&items, secs);
	
	// dhcp ip addr
	cbor_encode_text_string(&items, "ipv4", 4);
	int len = freertos_sprintf(
		buf, "%d.%d.%d.%d", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]
	);
	cbor_encode_text_string(&items, buf, len);
	
	// main settings
	CborEncoder map;
	cbor_encode_text_string(&items, "main", 4);
	cbor_encoder_create_map(&items, &map, 1);
	cbor_encode_text_string(&map, "macaddr", 7);
	uint8_t tmp[6];
	eeprom_get_mac_addr(tmp);
	len = freertos_sprintf(
		buf, "%02X:%02X:%02X:%02X:%02X:%02X", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5]
	);
	cbor_encode_text_string(&map, buf, len);
	cbor_encoder_close_container(&items, &map);

	// mqtt settings
	cbor_encode_text_string(&items, "mqtt", 4);
	cbor_encoder_create_map(&items, &map, 2);
	cbor_encode_text_string(&map, "host", 4);
	eeprom_mqtt_get_addr(tmp);
	len = freertos_sprintf(
		buf, "%d.%d.%d.%d", tmp[0], tmp[1], tmp[2], tmp[3]
	);
	cbor_encode_text_string(&map, buf, len);
	cbor_encode_text_string(&map, "port", 4);
	uint16_t port = eeprom_mqtt_get_port();
	cbor_encode_uint(&map, port);
	cbor_encoder_close_container(&items, &map);
	
	// switch status
	cbor_encode_text_string(&items, "switches", 8);
	cbor_encoder_create_map(&items, &map, 2);
	cbor_encode_text_string(&map, "switch1", 7);
	cbor_encode_boolean(&map, relay_fan_status_1());
	cbor_encode_text_string(&map, "switch2", 7);
	cbor_encode_boolean(&map, relay_fan_status_2());
	cbor_encoder_close_container(&items, &map);
	
	// sensor addresses
	CborEncoder array;
	uint8_t sensor_count = get_temperature_sensor_count();
	cbor_encode_text_string(&items, "sensors", 7);
	cbor_encoder_create_array(&items, &array, sensor_count);
	for (uint8_t i = 0; i < sensor_count; i++)
	{
		cbor_encoder_create_map(&array, &map, 5);
		struct ds18b20_desc *sensor = get_temperature_sensor_by_index(i);
		
		cbor_encode_text_string(&map, "addr", 4);
		cbor_encode_byte_string(&map, (uint8_t *) &sensor->addr, 8);

		int8_t sensor_id = eeprom_sensor_find_by_addr(sensor->addr);
		cbor_encode_text_string(&map, "indoor", 6);
		cbor_encode_boolean(&map, eeprom_sensor_get_indoor(sensor_id));
		
		cbor_encode_text_string(&map, "threshold", 9);
		cbor_encode_uint(&map, eeprom_sensor_get_security_threshold(sensor_id));

		cbor_encode_text_string(&map, "offset", 6);
		cbor_encode_int(&map, eeprom_sensor_get_offset(sensor_id));

		cbor_encode_text_string(&map, "name", 4);
		uint8_t name_len = eeprom_sensor_get_name(sensor_id, (const char *) &buf);
		if (name_len > 0)
			cbor_encode_text_string(&map, buf, name_len);
		else
			cbor_encode_null(&map);
		cbor_encoder_close_container(&array, &map);
	}
	cbor_encoder_close_container(&items, &array);
	cbor_encoder_close_container(&encoder, &items);

	msg.qos = QOS0;
	msg.retained = 1;
	msg.payload = mqtt_buf;
	msg.payloadlen = cbor_encoder_get_buffer_size(&encoder, mqtt_buf);
	MQTTPublish(client, "fancontrol/update\0", &msg);
}

static void send_mqtt_indoor_outdoor_update(MQTTClient *client)
{
	uart_write("MQTT: Send Temperature Difference Update\r\n", 42, 1000);

	uint16_t outdoor = get_temperature_avg_outdoor();
	uint16_t indoor = get_temperature_avg_indoor();

	CborEncoder encoder;
	cbor_encoder_init(&encoder, mqtt_buf, sizeof(mqtt_buf), 0);
	
	CborEncoder map;
	cbor_encoder_create_map(&encoder, &map, 2);
	
	cbor_encode_text_string(&map, "outdoor", 7);
	cbor_encode_uint(&map, outdoor);

	cbor_encode_text_string(&map, "indoor", 6);
	cbor_encode_uint(&map, indoor);
	
	cbor_encoder_close_container(&encoder, &map);

	msg.qos = QOS0;
	msg.retained = 0;
	msg.payload = mqtt_buf;
	msg.payloadlen = cbor_encoder_get_buffer_size(&encoder, mqtt_buf);
	MQTTPublish(client, "fancontrol/tempdiff\0", &msg);
}

static void send_mqtt_sensor_update(MQTTClient *client, struct ds18b20_desc *sensor)
{
	// Print uart message
	int len = freertos_sprintf(
		buf, 
		"MQTT: Send Sensor Update -> %02X%02X%02X%02X%02X%02X%02X%02X\r\n",
		(uint8_t) (sensor->addr >> 56),
		(uint8_t) (sensor->addr >> 48),
		(uint8_t) (sensor->addr >> 40),
		(uint8_t) (sensor->addr >> 32),
		(uint8_t) (sensor->addr >> 24),
		(uint8_t) (sensor->addr >> 16),
		(uint8_t) (sensor->addr >>  8),
		(uint8_t) (sensor->addr >>  0)
	);
	uart_write(buf, len, 1000);

	// Prepare topic name
	freertos_sprintf(
		buf, 
		"fancontrol/%02X%02X%02X%02X%02X%02X%02X%02X/temperature\0",
		(uint8_t) (sensor->addr >> 56),
		(uint8_t) (sensor->addr >> 48),
		(uint8_t) (sensor->addr >> 40),
		(uint8_t) (sensor->addr >> 32),
		(uint8_t) (sensor->addr >> 24),
		(uint8_t) (sensor->addr >> 16),
		(uint8_t) (sensor->addr >>  8),
		(uint8_t) (sensor->addr >>  0)
	);

	CborEncoder encoder;
	cbor_encoder_init(&encoder, mqtt_buf, sizeof(mqtt_buf), 0);
	cbor_encode_uint(&encoder, sensor->reading);
	
	msg.retained = 0;
	msg.payload = (void *) mqtt_buf;
	msg.payloadlen = cbor_encoder_get_buffer_size(&encoder, mqtt_buf);
	MQTTPublish(client, buf, &msg);
}

static void mqtt_send_updates(MQTTClient *client)
{
	// General status packet
	send_mqtt_status_update(client);
	
	// Indoor-Outdoor status packet
	send_mqtt_indoor_outdoor_update(client);
	
	uint8_t sensor_count = get_temperature_sensor_count();
	for (uint8_t i = 0; i < sensor_count; i++)
	{
		struct ds18b20_desc *sensor = get_temperature_sensor_by_index(i);
		if (!sensor->valid)
			continue;
		
		// Sensor status packet
		send_mqtt_sensor_update(client, sensor);
	}
}

static void msg_handler(MessageData *msg)
{
	led1_b(true);
	
	// uninteresting topic
	if (msg->topicName->lenstring.len < 19)
	{
		led1_b(false);
		return;
	}
	
	// main settings topic
	if (msg->topicName->lenstring.len == 19)
	{
		uart_write(msg->topicName->lenstring.data, msg->topicName->lenstring.len, 1000);
		uart_write("\r\n", 2, 1000);
	}
	else
	{
		int subtopic_len = msg->topicName->lenstring.len - 20;
		char *subtopic =  msg->topicName->lenstring.data + 20;

		memcpy(temp_subtopic, subtopic, subtopic_len);
		temp_subtopic[subtopic_len] = 0x00;

		if (strcmp("main/mac", temp_subtopic) == 0)
		{
			if (msg->message->payloadlen == 6)
			{
				uint8_t mac_addr[6];
				memcpy(mac_addr, msg->message->payload, msg->message->payloadlen);
				eeprom_set_mac_addr(mac_addr);
				send_mqtt_status_update(&client);
			}
		}
		else if (strcmp("main/factoryreset", temp_subtopic) == 0)
		{
			eeprom_initialize_storage();
			send_mqtt_status_update(&client);
			MQTTDisconnect(&client);
			__NVIC_SystemReset();
		}
		else if (strcmp("main/reboot", temp_subtopic) == 0)
		{
			MQTTDisconnect(&client);
			__NVIC_SystemReset();
		}
		else if (strcmp("mqtt/addr", temp_subtopic) == 0)
		{
			if (msg->message->payloadlen == 4)
			{
				uint8_t mqtt_addr[4];
				memcpy(mqtt_addr, msg->message->payload, msg->message->payloadlen);
				eeprom_mqtt_set_addr(mqtt_addr);
				send_mqtt_status_update(&client);
			}
		}
		else if (strcmp("mqtt/port", temp_subtopic) == 0)
		{
			if (msg->message->payloadlen == 2)
			{
				uint16_t mqtt_port = ((uint16_t) ((* (uint8_t *)msg->message->payload) & 0xFF) << 8) | (* (uint8_t *)(msg->message->payload + 1) & 0xFF);
				eeprom_mqtt_set_port(mqtt_port);
				send_mqtt_status_update(&client);
			}
		}
		else if (strncmp(temp_subtopic, "sensor/", 7) == 0)
		{
			if (subtopic_len >= 23)
			{
				onewire_addr_t addr = 0;
				for (uint8_t i = 0; i < 16;)
				{
					addr |= ((onewire_addr_t) (dehex(temp_subtopic[7 + i]) << 4) | dehex(temp_subtopic[7 + i + 1])) << (56 - i / 2 * 8);
					i += 2;
				}			
			
				int8_t sensor_id = eeprom_sensor_find_by_addr(addr);
				if (sensor_id == -1)
				{
					struct ds18b20_desc *sensor = get_temperature_sensor_by_addr(addr);
					if (sensor == NULL)
					{
						led1_b(false);
						return;
					}
				
					sensor_id = eeprom_sensor_find_first_unused_index();
					if (sensor_id == -1)
					{
						led1_b(false);
						return;
					}
				
					if (eeprom_sensor_assignment_slot(sensor_id))
					{
						eeprom_sensor_set_addr(sensor_id, addr);
					}
				}

				if (subtopic_len > 24)
				{
					memcpy(temp_subtopic, &subtopic[24], subtopic_len - 24);
					temp_subtopic[subtopic_len - 24] = 0x00;

					if (strcmp("offset", temp_subtopic) == 0)
					{
						int16_t offset = ((int16_t) ((* (uint8_t *)msg->message->payload) & 0xFF) << 8) | (* (uint8_t *)(msg->message->payload + 1) & 0xFF);					
						eeprom_sensor_set_offset(sensor_id, offset);
					}
					else if (strcmp("name", temp_subtopic) == 0)
					{
						uint8_t length = (* (uint8_t *)msg->message->payload) & 0xFF;
						memcpy(mqtt_buf, ((uint8_t *)msg->message->payload) + 1, length);
						eeprom_sensor_set_name(sensor_id, (const char *) mqtt_buf, length);
					}
					else if (strcmp("threshold", temp_subtopic) == 0)
					{
						uint16_t threshold = ((uint16_t) ((* (uint8_t *)msg->message->payload) & 0xFF) << 8) | (* (uint8_t *)(msg->message->payload + 1) & 0xFF);
						eeprom_sensor_set_security_threshold(sensor_id, threshold);
					}
					else if (strcmp("indoor", temp_subtopic) == 0)
					{
						uint8_t indoor = (* (uint8_t *)msg->message->payload) & 0xFF;
						eeprom_sensor_set_indoor(sensor_id, indoor == 1);
					}
				}
			}
		}
		else if (strcmp("fan/1/on", temp_subtopic) == 0)
		{
			relay_fan_speed_1(true);
			send_mqtt_status_update(&client);
		}
		else if (strcmp("fan/1/off", temp_subtopic) == 0)
		{
			relay_fan_speed_1(false);
			send_mqtt_status_update(&client);
		}
		else if (strcmp("fan/2/on", temp_subtopic) == 0)
		{
			relay_fan_speed_2(true);
			send_mqtt_status_update(&client);
		}
		else if (strcmp("fan/2/off", temp_subtopic) == 0)
		{
			relay_fan_speed_2(false);
			send_mqtt_status_update(&client);
		}
		else if (strcmp("fan/off", temp_subtopic) == 0)
		{
			relay_fan_speed_off();
			send_mqtt_status_update(&client);
		}
	}

	led1_b(false);
}

static void eth_task(void *pvParameters)
{
	(void) pvParameters;
	
	uint8_t reg = getVERSIONR();
	if (reg != 0x04)
	{
		// Error
	}
		
	DHCP_init(7, dhcp_buf);

	TimerInit(&timer_update);
	TimerCountdownMS(&timer_update, 60000);

	TimerInit(&timer_last_update);
	TimerCountdownMS(&timer_last_update, 60000);

	bool mqtt_connected = false;
	while (1)
	{
		DHCP_run();
		
		wizchip_getnetinfo(&net_info);
		if (!mqtt_connected && valid_ip_addr(&net_info))
		{
			int len = freertos_sprintf(buf, "MQTT: Local IP %d.%d.%d.%d\r\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
			uart_write(buf, len, 1000);
			
			len = freertos_sprintf(buf, "MQTT: Connect to %d.%d.%d.%d\r\n", target_ip[0], target_ip[1], target_ip[2], target_ip[3]);
			uart_write(buf, len, 1000);

			NewNetwork(&network, 0);
			ConnectNetwork(&network, target_ip, target_port);
			MQTTClientInit(&client, &network, 1000, mqtt_send_buf, sizeof(mqtt_send_buf), mqtt_recv_buf, sizeof(mqtt_recv_buf));
			
			vTaskDelay(ticks1s);

			len = freertos_sprintf(buf, "MQTT: Auth with %d.%d.%d.%d\r\n", target_ip[0], target_ip[1], target_ip[2], target_ip[3]);
			uart_write(buf, len, 1000);
			
			MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
			data.willFlag = 0;
			data.MQTTVersion = 3;
			data.keepAliveInterval = 60;
			data.cleansession = 1;
			data.clientID = client_id;
			
			int ret = MQTTConnect(&client, &data);
			if (ret == SUCCESSS)
			{
				mqtt_connected = true;
				
				MQTTSubscribe(&client, "fancontrol/settings\0", QOS0, &msg_handler);
				MQTTSubscribe(&client, "fancontrol/settings/#\0", QOS0, &msg_handler);
			}
			
			len = freertos_sprintf(buf, "MQTT: Connected to %d.%d.%d.%d\r\n", target_ip[0], target_ip[1], target_ip[2], target_ip[3]);
			uart_write(buf, len, 1000);
			TimerCountdownMS(&timer_update, 10000);
			send_mqtt_settings_update(&client);
		}
		else if (mqtt_connected)
		{
			MQTTYield(&client, 0);
			
			if (!network_is_connected(&network))
			{
				int len = freertos_sprintf(buf, "MQTT: Disconnect from %d.%d.%d.%d\r\n", target_ip[0], target_ip[1], target_ip[2], target_ip[3]);
				uart_write(buf, len, 1000);
				
				MQTTDisconnect(&client);
				network.disconnect(&network);
				mqtt_connected = false;
			}
		}

		if (mqtt_connected && TimerIsExpired(&timer_update))
		{
			mqtt_send_updates(&client);
			TimerCountdownMS(&timer_update, 60000);
			TimerCountdownMS(&timer_last_update, 180000);
		} else if (!mqtt_connected && TimerIsExpired(&timer_update))
		{
			DHCP_init(7, dhcp_buf);
			TimerCountdownMS(&timer_update, 60000);
		}
		
		vTaskDelay(ticks20ms);
	}
}

bool exceeded_expected_update_rate()
{
	return TimerIsExpired(&timer_last_update);
}

void lowlevel_init_eth_task()
{
	// Early W5500 initialization
	wiznet_port_initialize(&sem_eth_isr);

	uint8_t bufSize = 16;
	wizchip_init(&bufSize, &bufSize);

	wiz_PhyConf phyConf;
	wizphy_getphyconf(&phyConf);
	wizphy_setphypmode(PHY_POWER_NORM);
	
	setMR(MR_RST);
	setSn_MR(0, Sn_MR_TCP);
	setINTLEVEL(1);
	setSIMR(1);
	setPHYCFGR(0x58);
	setPHYCFGR(0xD8);
	setSn_IMR(0, (Sn_IR_RECV));
}

BaseType_t create_eth_task()
{
	if (task_eth_pid != NULL) return pdFAIL;
	return xTaskCreate(eth_task, "ETH", TASK_ETH_STACK_SIZE, NULL, 2, task_eth_pid);
}

void init_eth_task()
{
	wiz_NetInfo netInfo;
	netInfo.dhcp = NETINFO_DHCP;
	eeprom_get_mac_addr(netInfo.mac);
	wizchip_setnetinfo(&netInfo);

	eeprom_mqtt_get_addr(target_ip);
	target_port = eeprom_mqtt_get_port();
}

void kill_eth_task()
{
	if (task_eth_pid == NULL) return;
	vTaskDelete(task_eth_pid);
	task_eth_pid = NULL;
}
