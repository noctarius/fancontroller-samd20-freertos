/*
 * ds18b20.h
 */ 

#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "atmel_start.h"
#include "onewire.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ds18b20_desc {
	bool				avail;
	int32_t				reading;
	onewire_addr_t		addr;
	struct onewire_desc	*ow;
	bool				valid;
};

bool ds18b20_initiate_reading(struct ds18b20_desc *dev);

bool ds18b20_get_reading(struct ds18b20_desc *dev, int16_t offset);

#ifdef __cplusplus
}
#endif

#endif /* __DS18B20_H__ */