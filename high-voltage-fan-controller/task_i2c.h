/*
 * task_i2c.h
 */ 

#ifndef __TASK_I2C_H__
#define __TASK_I2C_H__

#include "atmel_start.h"
#include "ticks.h"

#ifdef __cplusplus
extern "C" {
#endif

// Message struct type
typedef struct i2c_msg
{
	uint8_t			addr;
	uint16_t		reg_addr;
	bool			read;
	bool			reg;
	uint8_t			*data;
	uint16_t		data_len;
	xTaskHandle		initiator;
} i2c_msg;

#define TASK_I2C_STACK_SIZE configMINIMAL_STACK_SIZE

#define MAX_FRAME_SIZE 16

#define ERR_I2C_RESP_BUF_TOO_SMALL	-1
#define ERR_I2C_QUEUE_FULL			-2

void i2c_write(const uint8_t addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis);

void i2c_write_reg(const uint8_t addr, const uint16_t reg_addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis);

void i2c_read(const uint8_t addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis);

void i2c_read_reg(const uint8_t addr, const uint16_t reg_addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis);

BaseType_t create_i2c_task();

void init_i2c_task();

void kill_i2c_task();

#ifdef __cplusplus
}
#endif

#endif /* __TASK_I2C_H__ */
