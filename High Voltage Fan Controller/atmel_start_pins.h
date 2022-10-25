/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAMD20 has 8 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7

#define UART_TX GPIO(GPIO_PORTA, 0)
#define UART_RX GPIO(GPIO_PORTA, 1)
#define ETH_INT GPIO(GPIO_PORTA, 2)
#define ETH_RST GPIO(GPIO_PORTA, 3)
#define MOSI GPIO(GPIO_PORTA, 4)
#define SCLK GPIO(GPIO_PORTA, 5)
#define MISO GPIO(GPIO_PORTA, 6)
#define ETH_SCS GPIO(GPIO_PORTA, 7)
#define SDA GPIO(GPIO_PORTA, 8)
#define SCL GPIO(GPIO_PORTA, 9)
#define ONEWIRE GPIO(GPIO_PORTA, 14)
#define SYS_CLK GPIO(GPIO_PORTA, 15)
#define SWITCH_FAN_S1 GPIO(GPIO_PORTA, 16)
#define SWITCH_FAN_S2 GPIO(GPIO_PORTA, 17)
#define LED_1_B GPIO(GPIO_PORTA, 18)
#define LED_1_R GPIO(GPIO_PORTA, 19)
#define LED_1_G GPIO(GPIO_PORTA, 22)
#define LED_2_B GPIO(GPIO_PORTA, 23)
#define LED_2_R GPIO(GPIO_PORTA, 24)
#define LED_2_G GPIO(GPIO_PORTA, 25)

#endif // ATMEL_START_PINS_H_INCLUDED
