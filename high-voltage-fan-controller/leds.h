
/*
 * leds.h
 */ 

#ifndef __LEDS_H__
#define __LEDS_H__

#include <stdbool.h>

void led1_r(bool v);

void led1_g(bool v);

void led1_b(bool v);

void led2_r(bool v);

void led2_g(bool v);

void led2_b(bool v);

void led1(bool r, bool g, bool b);

void led2(bool r, bool g, bool b);

void led1_off();

void led2_off();

void leds_off();

#endif /* __LEDS_H__ */

