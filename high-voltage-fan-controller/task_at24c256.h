/*
 * task_at24c256.h
 */ 

#ifndef __TASK_AT24C256_H__
#define __TASK_AT24C256_H__

#include "atmel_start.h"
#include "ticks.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_AT24C256_STACK_SIZE configMINIMAL_STACK_SIZE

#define ERR_AT24_QUEUE_FULL			-2

typedef struct at24_msg
{
    uint16_t        reg_addr;
    bool			read;
    uint8_t         *data;
    uint16_t        data_len;
    xTaskHandle		initiator;
} at24_msg;

void at24_write(const uint16_t reg_addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis);

void at24_read(const uint16_t reg_addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis);

BaseType_t create_at24c256_task();

void init_at24c256_task();

void kill_at24c256_task();

#ifdef __cplusplus
}
#endif

#endif /* __TASK_AT24C256_H__ */