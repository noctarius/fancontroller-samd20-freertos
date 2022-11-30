/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>

/*! The buffer size for USART */
#define USART_0_BUFFER_SIZE 16

struct spi_m_sync_descriptor  SPI_0;
struct usart_async_descriptor USART_0;

static uint8_t USART_0_buffer[USART_0_BUFFER_SIZE];

struct i2c_m_async_desc I2C_0;

struct wdt_descriptor WDT_0;

void EXTERNAL_IRQ_0_init(void)
{
	_gclk_enable_channel(EIC_GCLK_ID, CONF_GCLK_EIC_SRC);

	// Set pin direction to input
	gpio_set_pin_direction(ETH_INT, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(ETH_INT,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_UP);

	gpio_set_pin_function(ETH_INT, PINMUX_PA02A_EIC_EXTINT2);

	ext_irq_init();
}

void SPI_0_PORT_init(void)
{

	gpio_set_pin_level(MOSI,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(MOSI, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(MOSI, PINMUX_PA04D_SERCOM0_PAD0);

	gpio_set_pin_level(SCLK,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(SCLK, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(SCLK, PINMUX_PA05D_SERCOM0_PAD1);

	// Set pin direction to input
	gpio_set_pin_direction(MISO, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(MISO,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(MISO, PINMUX_PA06D_SERCOM0_PAD2);
}

void SPI_0_CLOCK_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM0);
	_gclk_enable_channel(SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC);
}

void SPI_0_init(void)
{
	SPI_0_CLOCK_init();
	spi_m_sync_init(&SPI_0, SERCOM0);
	SPI_0_PORT_init();
}

/**
 * \brief USART Clock initialization function
 *
 * Enables register interface and peripheral clock
 */
void USART_0_CLOCK_init()
{

	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM1);
	_gclk_enable_channel(SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC);
}

/**
 * \brief USART pinmux initialization function
 *
 * Set each required pin to USART functionality
 */
void USART_0_PORT_init()
{

	gpio_set_pin_function(UART_TX, PINMUX_PA00D_SERCOM1_PAD0);

	gpio_set_pin_function(UART_RX, PINMUX_PA01D_SERCOM1_PAD1);
}

/**
 * \brief USART initialization function
 *
 * Enables USART peripheral, clocks and initializes USART driver
 */
void USART_0_init(void)
{
	USART_0_CLOCK_init();
	usart_async_init(&USART_0, SERCOM1, USART_0_buffer, USART_0_BUFFER_SIZE, (void *)NULL);
	USART_0_PORT_init();
}

void I2C_0_PORT_init(void)
{

	gpio_set_pin_pull_mode(SDA,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(SDA, PINMUX_PA08D_SERCOM2_PAD0);

	gpio_set_pin_pull_mode(SCL,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(SCL, PINMUX_PA09D_SERCOM2_PAD1);
}

void I2C_0_CLOCK_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM2);
	_gclk_enable_channel(SERCOM2_GCLK_ID_CORE, CONF_GCLK_SERCOM2_CORE_SRC);
	_gclk_enable_channel(SERCOM2_GCLK_ID_SLOW, CONF_GCLK_SERCOM2_SLOW_SRC);
}

void I2C_0_init(void)
{
	I2C_0_CLOCK_init();
	i2c_m_async_init(&I2C_0, SERCOM2);
	I2C_0_PORT_init();
}

void WDT_0_CLOCK_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBA, WDT);
	_gclk_enable_channel(WDT_GCLK_ID, CONF_GCLK_WDT_SRC);
}

void WDT_0_init(void)
{
	WDT_0_CLOCK_init();
	wdt_init(&WDT_0, WDT);
}

void system_init(void)
{
	init_mcu();

	// GPIO on PA03

	gpio_set_pin_level(ETH_RST,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(ETH_RST, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(ETH_RST, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA07

	gpio_set_pin_level(ETH_SCS,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(ETH_SCS, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(ETH_SCS, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA14

	gpio_set_pin_level(ONEWIRE,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(ONEWIRE, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(ONEWIRE, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA15

	gpio_set_pin_level(SYS_CLK,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	// Set pin direction to output
	gpio_set_pin_direction(SYS_CLK, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(SYS_CLK, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA16

	gpio_set_pin_level(SWITCH_FAN_S1,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(SWITCH_FAN_S1, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(SWITCH_FAN_S1, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA17

	gpio_set_pin_level(SWITCH_FAN_S2,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(SWITCH_FAN_S2, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(SWITCH_FAN_S2, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA18

	gpio_set_pin_level(LED_1_B,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(LED_1_B, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(LED_1_B, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA19

	gpio_set_pin_level(LED_1_R,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(LED_1_R, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(LED_1_R, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA22

	gpio_set_pin_level(LED_1_G,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(LED_1_G, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(LED_1_G, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA23

	gpio_set_pin_level(LED_2_B,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(LED_2_B, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(LED_2_B, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA24

	gpio_set_pin_level(LED_2_R,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(LED_2_R, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(LED_2_R, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA25

	gpio_set_pin_level(LED_2_G,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(LED_2_G, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(LED_2_G, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA27

	// Set pin direction to input
	gpio_set_pin_direction(SENSE_S1, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(SENSE_S1,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(SENSE_S1, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA28

	// Set pin direction to input
	gpio_set_pin_direction(SENSE_S2, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(SENSE_S2,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(SENSE_S2, GPIO_PIN_FUNCTION_OFF);

	EXTERNAL_IRQ_0_init();

	SPI_0_init();
	USART_0_init();

	I2C_0_init();

	WDT_0_init();
}
