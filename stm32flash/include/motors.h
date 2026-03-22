#ifndef MOTORS_H
#define MOTORS_H

#include <stdint.h>
#include "gpio.h"
#include "i2c.h"
#include "usart.h"

#define INL3 PC8
#define INL4 PC9
#define INL2 PC11
#define INL1 PC10
#define INR3 PC12
#define INR1 PB3
#define INR2 PA10
#define INR4 PB5

#define DATA_BUF_SIZE 64

uint8_t* get_meas_pair(void);

void input_timer_init(void);

void output_timer_init(void);

void set_speed(uint16_t target);

void motor_wrapper(void* args);

void init_servo_TIM(void);

int lateral_left(void* args);

int lateral_right(void* args);

void servo(void* args);

int step(void* args);
void step2(int16_t args);

int rotate(void* args);
void rotate2(int16_t args);
void global_pos(void* args);
#endif // !MOTORS_H
