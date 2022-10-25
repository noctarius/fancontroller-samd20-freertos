/*
 * task_uart.h
 */ 

#ifndef __TASK_UART_H__
#define __TASK_UART_H__

#include "atmel_start.h"
#include "FreeRTOS.h"
#include "ticks.h"

#ifdef __cplusplus
extern "C" {
#endif

// Message struct type
typedef struct uart_msg
{
	uint8_t			*msg;
	uint8_t			msg_len;
	xTaskHandle		initiator;
} uart_msg;

#define TASK_UART_STACK_SIZE configMINIMAL_STACK_SIZE

#define ERR_UART_RESP_BUF_TOO_SMALL	-1
#define ERR_UART_QUEUE_FULL			-2

int16_t write_uart(const char *msg, const uint8_t msg_len, const uint16_t timeout_millis);

BaseType_t create_uart_task();

void init_uart_task();

void kill_uart_task();

#ifdef __cplusplus
}
#endif

#endif /* __TASK_UART_H__ */
