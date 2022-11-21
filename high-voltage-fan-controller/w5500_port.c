/*
 * w5500_port.c
 */ 

#include "w5500_port.h"
#include <FreeRTOS.h>
#include <atmel_start.h>

static struct io_descriptor *io_spi0;
static SemaphoreHandle_t *sem_eth_isr;

void wiznet_port_isr_handler(void)
{
	/*BaseType_t higherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(*sem_eth_isr, &higherPriorityTaskWoken);
	if (higherPriorityTaskWoken != pdFALSE)
	{
		portYIELD_FROM_ISR(higherPriorityTaskWoken);
	}*/
}

void wiznet_port_initialize(SemaphoreHandle_t *eth_isr_semaphore)
{
	// Store semaphore reference pointer
	sem_eth_isr = eth_isr_semaphore;
	
	// Configure SCS as output
    gpio_set_pin_direction(ETH_SCS, GPIO_DIRECTION_OUT);

	// Disable chip select
	gpio_set_pin_level(ETH_SCS, true);

	// Enable SPI bus
	spi_m_sync_enable(&SPI_0);
	spi_m_sync_get_io_descriptor(&SPI_0, &io_spi0);
	
	// Register callback functions
	reg_wizchip_cris_cbfunc(&wiznet_port_cris_enter, &wiznet_port_cris_exit);
	reg_wizchip_cs_cbfunc(&wiznet_port_cs_select, &wiznet_port_cs_deselect);
	reg_wizchip_spi_cbfunc(&wiznet_port_spi_readbyte, &wiznet_port_spi_writebyte);
	reg_wizchip_spiburst_cbfunc(&wiznet_port_spi_read_burst, &wiznet_port_spi_write_burst);
	
	// Enable interrupt handling
	ext_irq_enable(ETH_INT);
	ext_irq_disable(ETH_INT);
	ext_irq_register(ETH_INT, &wiznet_port_isr_handler);
	ext_irq_enable(ETH_INT);
	
	// Release reset pin
	wiznet_port_hard_reset();
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
    gpio_set_pin_level(ETH_SCS, false);
}

void wiznet_port_cs_deselect(void)
{
    gpio_set_pin_level(ETH_SCS, true);
}

uint8_t wiznet_port_spi_readbyte(void)
{
	uint8_t rx;
    io_read(io_spi0, &rx, 1);
	return rx;
}

void wiznet_port_spi_writebyte(uint8_t wb)
{
	io_write(io_spi0, &wb, 1);
}

void wiznet_port_spi_read_burst(uint8_t* buf, uint16_t len)
{
	io_read(io_spi0, buf, len);
}

void wiznet_port_spi_write_burst(uint8_t* buf, uint16_t len)
{
    io_write(io_spi0, buf, len);
}

void wiznet_port_hard_reset(void)
{
	wiznet_port_cris_enter();
	gpio_set_pin_level(ETH_RST, false);
	delay_us(500);
	gpio_set_pin_level(ETH_RST, true);
	delay_us(1000);
	wiznet_port_cris_exit();
}
