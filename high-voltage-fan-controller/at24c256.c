/*
 * at24c256.c
 */ 

#include "at24c256.h"
#include "task_i2c.h"

void at24c256_write(struct at24c256_desc *dev, const uint16_t reg_addr, const uint8_t *data, const uint16_t count)
{
	i2c_write_reg(dev->addr, reg_addr, data, count);
}

void at24c256_read(struct at24c256_desc *dev, const uint16_t reg_addr, const uint8_t *data, const uint16_t count)
{
	i2c_read_reg(dev->addr, reg_addr, data, count);
}
