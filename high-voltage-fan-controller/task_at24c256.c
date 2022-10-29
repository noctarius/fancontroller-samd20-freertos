/*
 * task_at24c256.c
 */ 

#include "task_at24c256.h"
#include "task_uart.h"
#include "at24c256.h"

// Internal task pid
static xTaskHandle task_at24c256_pid = NULL;

// Internal message queue
static QueueHandle_t msg_queue = NULL;

static void at24_task(void *pvParameters)
{
    (void) pvParameters;

    uart_write("Initialize EEPROM...\r\n", 22, 1000);

    struct at24c256_desc dev =
    {
        addr: 0x50,
    };
    

	BaseType_t ret;
    struct at24_msg msg;
    while (1)
    {
        if ((ret = xQueueReceive(msg_queue, &msg, 0)) == pdPASS)
        {
            if (msg.read)
            {
                at24_read(msg.reg_addr, msg.data, msg.data_len, 1000);

            } else {
                at24_write(msg.reg_addr, msg.data, msg.data_len, 1000);
            }
        }
    }
}

static void at24_enqueue(bool read, const uint16_t reg_addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis)
{
    // Store for notification
	xTaskHandle initiator = xTaskGetCurrentTaskHandle();

    struct at24_msg *msg = &(struct at24_msg)
    {
        .reg_addr = reg_addr,
        .read = read,
        .data = (uint8_t *) data,
        .data_len = count,
        .initiator = initiator
    };

    if (xQueueSend(msg_queue, &msg, ticks100ms) != pdPASS)
	{
		// Queue full
		return ERR_AT24_QUEUE_FULL;
	}

	// Wait for full response received
	uint32_t notificationValue = ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(timeout_millis - 100));
	if (notificationValue == 1)
	{
		return;
	}
	
	// Timeout
	return ERR_TIMEOUT;
}

void at24_write(const uint16_t reg_addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis)
{
    at24_enqueue(false, reg_addr, data, count, timeout_millis);
}

void at24_read(const uint16_t reg_addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis)
{
    at24_enqueue(true, reg_addr, data, count, timeout_millis);
}

BaseType_t create_at24c256_task()
{
	if (task_at24c256_pid != NULL) return pdFAIL;
	return xTaskCreate(at24_task, "SELFTEST", TASK_AT24C256_STACK_SIZE, NULL, 2, task_at24c256_pid);	
}

void init_at24c256_task()
{

}

void kill_at24c256_task()
{
	if (task_at24c256_pid == NULL) return;
	vTaskDelete(task_at24c256_pid);
	task_at24c256_pid = NULL;
}
