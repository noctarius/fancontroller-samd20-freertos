/*
 * at24c256.c
 */ 

#include "at24c256.h"
#include "task_i2c.h"

inline void at24_write(struct at24c256_desc *dev, const uint16_t reg_addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis)
{
	i2c_write_reg(dev->addr, reg_addr, data, count, timeout_millis);
}

inline void at24_read(struct at24c256_desc *dev, const uint16_t reg_addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis)
{
	i2c_read_reg(dev->addr, reg_addr, data, count, timeout_millis);
}
