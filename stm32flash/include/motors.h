#ifndef MOTORS_H
#define MOTORS_H

#include <stdint.h>
#include "gpio.h"

#define INL1 PC8
#define INL2 PC9
#define INL3 PC11
#define INL4 PC10
#define INR1 PC12
#define INR2 PB3
#define INR3 PB4
#define INR4 PB5

void input_timer_init(void);

void output_timer_init(void);

void set_speed(uint16_t target);

void test_toggle(void* args);

#endif // !MOTORS_H
