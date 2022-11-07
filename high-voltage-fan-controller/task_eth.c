
/*
 * task_eth.c
 */ 

#include "task_eth.h"
#include "dhcp.h"
#include "mqtt_interface.h"
#include "MQTTClient.h"

// Internal task pid
static xTaskHandle task_eth_pid = NULL;
static char buf[32];
static uint8_t mqtt_send_buf[100];
static uint8_t mqtt_recv_buf[256];
static uint8_t dhcp_buf[548];

static SemaphoreHandle_t sem_eth_isr;
static Network network;
static MQTTClient client;

static void clear_isr()
{
	setSn_IR(0, 0x1F);
	setSIR(0);
}

static void eth_task(void *pvParameters)
{
	(void) pvParameters;
	
	uint8_t reg = getVERSIONR();
	if (reg != 0x04)
	{
		// Error
	}
		
	wiz_NetInfo netInfo;
	DHCP_init(7, &dhcp_buf[0]);

	bool mqttConnected = false;
	while (1)
	{
		DHCP_run();
		
		wizchip_getnetinfo(&netInfo);
		int len = freertos_sprintf(&buf[0], "dhcp: %d.%d.%d.%d\r\n", netInfo.ip[0], netInfo.ip[1], netInfo.ip[2], netInfo.ip[3]);
		uart_write(buf, len, 1000);
		
		if (!mqttConnected && netInfo.ip[0] != 0x00 && netInfo.ip[1] != 0x00 && netInfo.ip[2] != 0x00 && netInfo.ip[3] != 0x00)
		{
			uint8_t targetIp[4];
			targetIp[0] = 10;
			targetIp[1] = 96;
			targetIp[2] = 0;
			targetIp[3] = 71;
			
			NewNetwork(&network, 0);
			ConnectNetwork(&network, targetIp, 1883);
			MQTTClientInit(&client, &network, 1000, &mqtt_send_buf, 100, &mqtt_recv_buf, 256);
			
			MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
			data.willFlag = 0;
			data.MQTTVersion = 3;
			data.keepAliveInterval = 60;
			data.cleansession = 1;
			
			int ret = MQTTConnect(&client, &data);
			if (ret == SUCCESSS)
			{
				mqttConnected = true;
			}
		}
		else if (mqttConnected)
		{
			MQTTYield(&client, 60);
		}
		
		/*if (xSemaphoreTake(sem_eth_isr, 0) != pdTRUE)
		{
			clear_isr();
		}*/
		
		vTaskDelay(ticks1s);
	}
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
	
	wiz_NetInfo netInfo;
	netInfo.dhcp = NETINFO_DHCP;
	netInfo.mac[0] = 0x7A;
	netInfo.mac[1] = 0xD3;
	netInfo.mac[2] = 0xF0;
	netInfo.mac[3] = 0x7C;
	netInfo.mac[4] = 0x0C;
	netInfo.mac[5] = 0x2E;
	wizchip_setnetinfo(&netInfo);
}

BaseType_t create_eth_task()
{
	if (task_eth_pid != NULL) return pdFAIL;
	return xTaskCreate(eth_task, "ETH", TASK_ETH_STACK_SIZE, NULL, 2, task_eth_pid);
}

void init_eth_task()
{
}

void kill_eth_task()
{
	if (task_eth_pid == NULL) return;
	vTaskDelete(task_eth_pid);
	task_eth_pid = NULL;
}
