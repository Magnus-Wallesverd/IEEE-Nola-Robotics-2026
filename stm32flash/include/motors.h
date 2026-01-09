#ifndef MOTORS_H
#define MOTORS_H

#include <stdint.h>


void input_timer_init(void);

void output_timer_init(void);

void set_speed(uint16_t target);
void motorgym(uint16_t target);

void test_toggle(void);

#endif // !MOTORS_H
