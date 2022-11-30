
/*
 * leds.c
 */ 

#include <atmel_start.h>
#include "leds.h"

inline void led1_r(bool v)
{
	gpio_set_pin_level(LED_1_R, !v);
}

inline bool led1_status_r()
{
	return gpio_get_pin_level(LED_1_R);
}

inline void led1_g(bool v)
{
	gpio_set_pin_level(LED_1_G, !v);
}

inline bool led1_status_g()
{
	return gpio_get_pin_level(LED_1_G);
}

inline void led1_b(bool v)
{
	gpio_set_pin_level(LED_1_B, !v);
}

inline bool led1_status_b()
{
	return gpio_get_pin_level(LED_1_B);
}

inline void led2_r(bool v)
{
	gpio_set_pin_level(LED_2_R, !v);
}

inline bool led2_status_r()
{
	return gpio_get_pin_level(LED_2_R);
}

inline void led2_g(bool v)
{
	gpio_set_pin_level(LED_2_G, !v);
}

inline bool led2_status_g()
{
	return gpio_get_pin_level(LED_2_G);
}

inline void led2_b(bool v)
{
	gpio_set_pin_level(LED_2_B, !v);
}

inline bool led2_status_b()
{
	return gpio_get_pin_level(LED_2_B);
}

inline void led1(bool r, bool g, bool b)
{
	led1_r(r);
	led1_g(g);
	led1_b(b);
}

inline void led2(bool r, bool g, bool b)
{
	led2_r(r);
	led2_g(g);
	led2_b(b);
}

inline void led1_off()
{
	led1(false, false, false);
}

inline void led2_off()
{
	led2(false, false, false);
}

inline void leds_off()
{
	led1_off();
	led2_off();
}
