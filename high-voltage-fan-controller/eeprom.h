
/*
 * eeprom.h
 */ 

#ifndef __EEPROM_H__
#define __EEPROM_H__


#ifdef __cplusplus
extern "C" {
#endif

struct __attribute__((__packed__)) eeprom_desc
{
	uint8_t header[4];
	uint8_t mac_addr[6];
	uint8_t ts1_addr[8];
	uint8_t ts2_addr[8];
	uint8_t ts3_addr[8];
	uint8_t ts4_addr[8];
	uint8_t ts5_addr[8];
	uint16_t ts1_thld;
	uint16_t ts2_thld;
	uint16_t ts3_thld;
	uint16_t ts4_thld;
	uint16_t ts5_thld;
	struct __attribute__((__packed__)) ts_location
	{
		bool ts1_indoor: 1;
		bool ts2_indoor: 1;
		bool ts3_indoor: 1;
		bool ts4_indoor: 1;
		bool ts5_indoor: 1;
		uint8_t reserved: 3;
	} ts_location;
} eeprom_desc;

#ifdef __cplusplus
}
#endif

#endif
