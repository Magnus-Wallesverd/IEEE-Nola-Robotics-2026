#ifndef MOTORS_H
#define MOTORS_H

#include <stdint.h>
#include "gpio.h"

#define IN3 PC8
#define IN4 PC8

void input_timer_init(void);

void output_timer_init(void);

void set_speed(uint16_t target);
void motorgym(uint16_t target);

void test_toggle(void* args);

#endif // !MOTORS_H
