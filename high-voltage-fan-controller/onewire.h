/*
 * onewire.h
 */ 

#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__

#include <atmel_start.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t onewire_addr_t;

typedef struct
{
	struct onewire_desc *dev;
	uint8_t rom_no[8];
	uint8_t last_discrepancy;
	bool last_device_found;
} onewire_search_t;

#define ONEWIRE_NONE ((onewire_addr_t)(0xffffffffffffffffLL))

struct onewire_desc
{
	const uint8_t pin;
};

bool onewire_reset(struct onewire_desc *const dev);

bool onewire_select(struct onewire_desc *const dev, const onewire_addr_t addr);

bool onewire_skip_rom(struct onewire_desc *const dev);

bool onewire_write(struct onewire_desc *const dev, uint8_t v);

bool onewire_write_bytes(struct onewire_desc *const dev, const uint8_t *buf, size_t count);

int onewire_read(struct onewire_desc *const dev);

bool onewire_read_bytes(struct onewire_desc *const dev, uint8_t *buf, size_t count);

bool onewire_power(struct onewire_desc *const dev);

void onewire_depower(struct onewire_desc *const dev);

void onewire_search_start(struct onewire_desc *const dev, onewire_search_t *search);

void onewire_search_prefix(onewire_search_t *search, uint8_t family_code);

onewire_addr_t onewire_search_next(onewire_search_t *search);

uint8_t onewire_crc8(const uint8_t *data, uint8_t len);

bool onewire_check_crc16(const uint8_t *data, size_t len, const uint8_t *inverted_crc, uint16_t crc_iv);

uint16_t onewire_crc16(const uint8_t *data, size_t len, uint16_t crc_iv);

#ifdef __cplusplus
}
#endif

#endif /* __ONEWIRE_H__ */