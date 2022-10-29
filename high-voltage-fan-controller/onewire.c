/*
 * onewire.c
 *
 * Based one:
 *  - https://github.com/UncleRus/esp-idf-lib/blob/master/components/onewire/onewire.c
 *  - https://github.com/mongoose-os-libs/onewire/blob/master/src/mgos_onewire.c
 */ 

#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <atmel_start.h>
#include "onewire.h"

#define ONEWIRE_SELECT_ROM 0x55
#define ONEWIRE_SKIP_ROM   0xcc
#define ONEWIRE_SEARCH     0xf0

static const uint8_t _crc_lookup_table[] = {
	0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
	157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
	35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
	190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
	70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
	219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
	101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
	248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
	140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
	17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
	175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
	50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
	202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
	87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
	233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
	116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

static inline bool _onewire_wait_for_bus(struct onewire_desc *const dev, int max_wait)
{
	taskENTER_CRITICAL();
	for (int i = 0; i < ((max_wait + 4) / 5); i++)
	{
		if (gpio_get_pin_level(dev->pin))
			break;

		delay_us(5);
	}

	bool state = gpio_get_pin_level(dev->pin);
	delay_us(1);
	taskEXIT_CRITICAL();
	return state;
}

static void _onewire_prepare_input(struct onewire_desc *const dev)
{
	gpio_set_pin_direction(dev->pin, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(dev->pin, GPIO_PULL_OFF);
}

static void _onewire_prepare_output(struct onewire_desc *const dev)
{
	gpio_set_pin_direction(dev->pin, GPIO_DIRECTION_OUT);
}

static bool _onewire_write_bit(struct onewire_desc *const dev, bool bit)
{
	_onewire_prepare_input(dev);
	
	if (!_onewire_wait_for_bus(dev, 10))
		return false;
		
	taskENTER_CRITICAL();
	_onewire_prepare_output(dev);
	gpio_set_pin_level(dev->pin, false);
	delay_us(bit ? 6 : 60);
	gpio_set_pin_level(dev->pin, true);
	delay_us(bit ? 64 : 10);
	taskEXIT_CRITICAL();
	
	return true;
}

static int _onewire_read_bit(struct onewire_desc *const dev)
{
	_onewire_prepare_input(dev);
	
	if (!_onewire_wait_for_bus(dev, 10))
		return -1;

	int res;	
	taskENTER_CRITICAL();
	_onewire_prepare_output(dev);
	gpio_set_pin_level(dev->pin, false);
	delay_us(6);
	_onewire_prepare_input(dev);
	delay_us(9);
	res = gpio_get_pin_level(dev->pin);
	delay_us(55);
	taskEXIT_CRITICAL();
	return res;
}

bool onewire_reset(struct onewire_desc *const dev)
{
	if (dev == NULL) return false;

	_onewire_prepare_input(dev);

	if (!_onewire_wait_for_bus(dev, 250))
		return false;

	taskENTER_CRITICAL();
	_onewire_prepare_output(dev);
	gpio_set_pin_level(dev->pin, false);

	delay_us(480);

	bool res;
	_onewire_prepare_input(dev);
	delay_us(70);
	res = !gpio_get_pin_level(dev->pin);
	taskEXIT_CRITICAL();

	if (!_onewire_wait_for_bus(dev, 410))
	return false;

	return res;
}

bool onewire_select(struct onewire_desc *const dev, onewire_addr_t addr)
{
	if (dev == NULL) return false;
	
	if (!onewire_write(dev, ONEWIRE_SELECT_ROM))
		return false;

	for (int i = 0; i < 8; i++)
	{
		if (!onewire_write(dev, addr & 0xff))
			return false;
		
		addr >>= 8;
	}
	
	return true;
}

bool onewire_skip_rom(struct onewire_desc *const dev)
{
	if (dev == NULL) return false;
	
	return onewire_write(dev, ONEWIRE_SKIP_ROM);
}

bool onewire_write(struct onewire_desc *const dev, uint8_t v)
{
	if (dev == NULL) return false;
	
	taskENTER_CRITICAL();
	for (uint8_t mask = 0x01; mask; mask <<= 1)
		if (!_onewire_write_bit(dev, (mask & v) ? 1 : 0))
		{
			taskEXIT_CRITICAL();
			return false;
		}

	taskEXIT_CRITICAL();
	return true;
}

bool onewire_write_bytes(struct onewire_desc *const dev, const uint8_t *buf, size_t count)
{
	if (dev == NULL) return false;
	
	for (size_t i = 0; i < count; i++)
		if (!onewire_write(dev, buf[i]))
			return false;

	return true;
}

int onewire_read(struct onewire_desc *const dev)
{
	if (dev == NULL) return -1;
	
	int res = 0;
	for (uint8_t mask = 0x01; mask; mask <<= 1)
	{
		int bit = _onewire_read_bit(dev);
		if (bit < 0)
			return -1;
		else if (bit)
			res |= mask;
	}
	
	return res;
}

bool onewire_read_bytes(struct onewire_desc *const dev, uint8_t *buf, size_t count)
{
	if (dev == NULL) return false;
	
	for (size_t i = 0; i < count; i++)
	{
		int v = onewire_read(dev);
		if (v < 0)
			return false;
			
		buf[i] = v;
	}
	
	return true;
}

bool onewire_power(struct onewire_desc *const dev)
{
	if (dev == NULL) return false;
	
	_onewire_prepare_input(dev);
		
	if (!_onewire_wait_for_bus(dev, 10))
		return false;
		
	_onewire_prepare_output(dev);
	gpio_set_pin_level(dev->pin, 1);
	
	return true;
}

void onewire_depower(struct onewire_desc *const dev)
{
	if (dev == NULL) return;
	_onewire_prepare_input(dev);
}

void onewire_search_start(struct onewire_desc *const dev, onewire_search_t *search)
{
	memset(search, 0, sizeof(*search));
	search->dev = dev;
}

void onewire_search_prefix(onewire_search_t *search, uint8_t family_code)
{
	search->rom_no[0] = family_code;
	for(uint8_t i = 1; i < 8; i++)
		search->rom_no[i] = 0;
		
	search->last_discrepancy = 64;
	search->last_device_found = false;
}

onewire_addr_t onewire_search_next(onewire_search_t *search)
{
	uint8_t id_bit_number = 1, last_zero = 0, rom_byte_number = 0, rom_byte_mask = 1, search_result;
	int id_bit, cmp_id_bit, dir;
	
	if (!search->last_device_found)
	{
		if (!onewire_reset(search->dev))
		{
			search->last_discrepancy = 0;
			search->last_device_found = false;
			return ONEWIRE_NONE;
		}

		onewire_write(search->dev, ONEWIRE_SEARCH);

		do
		{
			id_bit = _onewire_read_bit(search->dev);
			cmp_id_bit = _onewire_read_bit(search->dev);

			if ((id_bit == 1) && (cmp_id_bit == 1))
				break;

			if (id_bit != cmp_id_bit)
				dir = id_bit;
			else
			{
				if (id_bit_number < search->last_discrepancy)
					dir = ((search->rom_no[rom_byte_number] & rom_byte_mask) > 0);
				else
					dir = (id_bit_number == search->last_discrepancy);

				if (!dir)
					last_zero = id_bit_number;
			}

			if (dir)
				search->rom_no[rom_byte_number] |= rom_byte_mask;
			else
				search->rom_no[rom_byte_number] &= ~rom_byte_mask;

			_onewire_write_bit(search->dev, dir);

			id_bit_number++;
			rom_byte_mask <<= 1;

			if (rom_byte_mask == 0)
			{
				rom_byte_number++;
				rom_byte_mask = 1;
			}
		} while (rom_byte_number < 8);

		if (!(id_bit_number < 65))
		{
			search->last_discrepancy = last_zero;

			if (search->last_discrepancy == 0)
				search->last_device_found = true;

			search_result = 1;
		}
	}

	if (search_result && search->rom_no[0] &&
		(search->rom_no[7] == onewire_crc8(search->rom_no, 7)))
	{
		onewire_addr_t addr = 0;
		for (rom_byte_number = 7; rom_byte_number >= 0; rom_byte_number--)
			addr = (addr << 8) | search->rom_no[rom_byte_number];

		return addr;
	}

	search->last_discrepancy = 0;
	search->last_device_found = false;
	return ONEWIRE_NONE;
}

uint8_t onewire_crc8(const uint8_t *data, uint8_t len)
{
	uint8_t crc = 0x00;

	while (len-- > 0)
	{
		crc = _crc_lookup_table[crc ^ *data++];
	}

	return crc;
}

bool onewire_check_crc16(const uint8_t *data, size_t len, const uint8_t *inverted_crc, uint16_t crc_iv)
{
	uint16_t crc = ~onewire_crc16(data, len, crc_iv);
	return (crc & 0xFF) == inverted_crc[0] && (crc >> 8) == inverted_crc[1];
}

uint16_t onewire_crc16(const uint8_t *data, size_t len, uint16_t crc_iv)
{
	uint16_t crc = crc_iv;
	static const uint8_t oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
		
	for (uint16_t i = 0; i < len; i++)
	{
		uint16_t cdata = data[i];
		cdata = (cdata ^ crc) & 0xFF;
		crc >>= 8;
		
		if (oddparity[cdata & 0x0F] ^ oddparity[cdata >> 4])
			crc ^= 0xC001;
			
		cdata <<= 6;
		crc ^= cdata;
		cdata <<= 1;
		crc ^= cdata;
	}
	return crc;
}
