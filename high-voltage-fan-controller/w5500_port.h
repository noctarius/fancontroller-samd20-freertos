/*
 * w5500_port.h
 */ 

#ifndef __W5500_PORT_H__
#define __W5500_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "wizchip_conf.h"

void wiznet_port_initialize(SemaphoreHandle_t *eth_isr_semaphore);
void wiznet_port_cris_enter(void);
void wiznet_port_cris_exit(void);
void wiznet_port_cs_select(void);
void wiznet_port_cs_deselect(void);
uint8_t wiznet_port_spi_readbyte(void);
void wiznet_port_spi_writebyte(uint8_t wb);
void wiznet_port_spi_read_burst(uint8_t* buf, uint16_t len);
void wiznet_port_spi_write_burst(uint8_t* buf, uint16_t len);
void wiznet_port_hard_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* __W5500_PORT_H__ */
