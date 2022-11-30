
/*
 * leds.h
 */ 

#ifndef __LEDS_H__
#define __LEDS_H__

#include <stdbool.h>

void led1_r(bool v);

bool led1_status_r();

void led1_g(bool v);

bool led1_status_g();

void led1_b(bool v);

bool led1_status_b();

void led2_r(bool v);

bool led2_status_r();

void led2_g(bool v);

bool led2_status_g();

void led2_b(bool v);

bool led2_status_b();

void led1(bool r, bool g, bool b);

void led2(bool r, bool g, bool b);

void led1_off();

void led2_off();

void leds_off();

#endif /* __LEDS_H__ */

