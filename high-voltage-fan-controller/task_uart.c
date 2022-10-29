/*
 * task_uart.c
 */ 

#include "task_uart.h"
#include "messages.h"
#include <samd20e18.h>

// Internal task pid
static xTaskHandle task_uart_pid = NULL;

// Internal uart io instance
static struct io_descriptor *io_usart0 = NULL;

// Internal message queue
static QueueHandle_t msg_queue = NULL;

// Currently handled message
static struct uart_msg *msg = NULL;

// Internal state variables
static volatile bool tx_in_progress = false;

static void tx_cb_USART_0(const struct usart_async_descriptor *const io_descr)
{
	if (msg != NULL)
	{
		tx_in_progress = false;
	}
}

static void rx_cb_USART_0(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
}

static void err_cb_USART_0(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
	if (msg != NULL)
	{
		tx_in_progress = false;
	}
}

static void uart_write_0(const char *data, const uint8_t len)
{
	if (len > 0)
	{
		tx_in_progress = true;
		io_write(io_usart0, (uint8_t *)data, len);
		while (SERCOM1->USART.INTFLAG.bit.TXC == 0)
			vTaskDelay(ticks20ms);
	}
}

static int16_t uart_read(uint8_t *target, uint8_t len)
{
	// Non blocking
	uint8_t retval = usart_async_is_rx_not_empty(&USART_0);
	if (retval == 0)
	{
		return retval;
	}
	return io_read(io_usart0, target, len);
}

static void uart_task(void *pvParameters)
{
	(void) pvParameters;
	
	uart_write_0(MSG_UART_START, MSG_UART_START_LEN);
	
	BaseType_t ret;
	while (1)
	{
		if (msg == NULL)
		{
			if ((ret = xQueueReceive(msg_queue, &msg, 0)) == pdPASS)
			{
				uart_write_0(msg->msg, msg->msg_len);
				
				// Wait for write to finish
				while (tx_in_progress)
					vTaskDelay(ticks20ms);

				xTaskNotifyGive(msg->initiator);
				msg = NULL;
			}
		}
		
		vTaskDelay(ticks100ms);
	}
	
	vTaskDelete(NULL);
}

int16_t uart_write(char *msg, const uint8_t msg_len, const uint16_t timeout_millis)
{
	xTaskHandle initiator = xTaskGetCurrentTaskHandle();
	
	struct uart_msg *new_msg = &(struct uart_msg) {
		.msg = msg,
		.msg_len = msg_len,
		.initiator = initiator
	};
	
	if (xQueueSend(msg_queue, &new_msg, ticks100ms) != pdPASS)
	{
		return ERR_UART_QUEUE_FULL;
	}
	
	uint32_t notificationValue = ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(timeout_millis - 100));
	if (notificationValue == 1)
	{
		return 1;
	}
	
	return ERR_TIMEOUT;
}

BaseType_t create_uart_task()
{
	if (task_uart_pid != NULL) return pdFAIL;
	return xTaskCreate(uart_task, "UART", TASK_UART_STACK_SIZE, NULL, 2, task_uart_pid);
}

void init_uart_task()
{
	msg_queue = xQueueCreate(4, sizeof(struct uart_msg *));
	usart_async_register_callback(&USART_0, USART_ASYNC_TXC_CB, tx_cb_USART_0);
	usart_async_register_callback(&USART_0, USART_ASYNC_RXC_CB, rx_cb_USART_0);
	usart_async_register_callback(&USART_0, USART_ASYNC_ERROR_CB, err_cb_USART_0);
	usart_async_get_io_descriptor(&USART_0, &io_usart0);
	usart_async_enable(&USART_0);
}

void kill_uart_task()
{
	if (task_uart_pid == NULL) return;
	vTaskDelete(task_uart_pid);
	task_uart_pid = NULL;
}
