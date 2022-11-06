/*
 * w5500_port.c
 */ 

#include "w5500_port.h"
#include <FreeRTOS.h>
#include <atmel_start.h>

void wiznet_port_initialize(void)
{
    gpio_set_pin_direction(ETH_SCS, GPIO_DIRECTION_IN);
}

void wiznet_port_cris_enter(void)
{
    taskENTER_CRITICAL();
}

void wiznet_port_cris_exit(void)
{
    taskEXIT_CRITICAL();
}

void wiznet_port_cs_select(void)
{
    gpio_set_pin_level(ETH_SCS, true);
}

void wiznet_port_cs_deselect(void)
{
    gpio_set_pin_level(ETH_SCS, false);
}

uint8_t wiznet_port_spi_readbyte(void)
{
    SPI_0
    spi
}

void wiznet_port_spi_writebyte(uint8_t wb)
{

}

void wiznet_port_spi_read_burst(uint8_t* buf, uint16_t len)
{

}

void wiznet_port_spi_write_burst(uint8_t* buf, uint16_t len)
{
    
}
