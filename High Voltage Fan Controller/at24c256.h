/*
 * at24c256.h
 */ 

#ifndef __AT24C256_H__
#define __AT24C256_H__

#include "atmel_start.h"

#ifdef __cplusplus
extern "C" {
#endif

struct at24c256_desc {
	const uint8_t addr;
};

void at24c256_write(struct at24c256_desc *dev, const uint16_t reg_addr, const uint8_t *data, const uint16_t count);

void at24c256_read(struct at24c256_desc *dev, const uint16_t reg_addr, const uint8_t *data, const uint16_t count);

#ifdef __cplusplus
}
#endif

#endif /* __AT24C256_H__ */
