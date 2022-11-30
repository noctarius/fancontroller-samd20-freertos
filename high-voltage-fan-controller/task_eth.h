
/*
 * task_eth.h
 */ 

#ifndef __TASK_ETH_H__
#define __TASK_ETH_H__

#include "atmel_start.h"
#include "ticks.h"
#include "w5500_port.h"
#include "printf_freertos.h"
#include "task_uart.h"
#include "socket.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_ETH_STACK_SIZE 340

bool exceeded_expected_update_rate();

void lowlevel_init_eth_task();

BaseType_t create_eth_task();

void init_eth_task();

void kill_eth_task();

#ifdef __cplusplus
}
#endif

#endif /* __TASK_ETH_H__ */
