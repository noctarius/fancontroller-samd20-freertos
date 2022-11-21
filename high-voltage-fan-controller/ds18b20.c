/*
 * ds18b20.c
 */ 

#include "ds18b20.h"
#include "eeprom.h"

bool ds18b20_initiate_reading(struct ds18b20_desc *dev)
{
	if (dev == NULL) return false;
	
	if (!onewire_reset(dev->ow)) return false;
	
	if (!onewire_select(dev->ow, dev->addr)) return false;
	
	if (!onewire_write(dev->ow, 0x44)) return false;
	
	return true;
}

bool ds18b20_get_reading(struct ds18b20_desc *dev, int16_t offset)
{
	if (dev == NULL) return false;
	
	if (!onewire_reset(dev->ow)) return false;
	
	if (!onewire_select(dev->ow, dev->addr)) return false;
	
	if (!onewire_write(dev->ow, 0xBE)) return false;
	
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
	
	uint8_t crc = onewire_crc8(scratchpad, 8);
	
	if (crc != scratchpad[8])
	{
		dev->reading = 0;
		dev->reading = false;
		return false;
	}
	
	dev->valid = true;
	double reading = raw * 0.0625;
	dev->reading = ((uint16_t) (reading * 100)) + offset;
	
	if (reading < -25. || reading > 125.)
	{
		dev->reading = 0;
		dev->valid = false;
		return false;
	}
	return true;
}
