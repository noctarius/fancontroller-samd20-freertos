/*
 * ds18b20.c
 */ 

#include "ds18b20.h"

bool ds18b20_initiate_reading(struct ds18b20_desc *dev)
{
	if (dev == NULL) return false;
	
	if (!onewire_reset(dev->ow)) return false;
	
	if (!onewire_select(dev->ow, dev->addr)) return false;
	
	if (!onewire_write(dev->ow, 0x44)) return false;
	
	return true;
}

bool ds18b20_get_reading(struct ds18b20_desc *dev)
{
	if (dev == NULL) return false;
	
	if (!onewire_reset(dev->ow)) return false;
	
	if (!onewire_select(dev->ow, dev->addr)) return false;
	
	if (!onewire_write(dev->ow, 0x44)) return false;
	
	uint8_t scratchpad[9];
	if (!onewire_read_bytes(dev->ow, scratchpad, 9)) return false;
	
	int16_t raw = (scratchpad[1] << 8) | scratchpad[0];
	char cfg = (scratchpad[4] & 0x60);
	
	if (cfg == 0x00)
		raw = raw & ~7;
	else if (cfg == 0x20)
		raw = raw & ~3;
	else if (cfg == 0x40)
		raw = raw & ~1;
		
	dev->valid = true;
	dev->reading = raw * 0.0625;
	if (dev->reading < -25 || dev->reading > 125)
	{
		dev->reading = 0;
		dev->valid = false;
	}
	return true;
}
