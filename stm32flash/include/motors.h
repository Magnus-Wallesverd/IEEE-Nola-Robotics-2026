#ifndef MOTORS_H
#define MOTORS_H

#include <stdint.h>
#include "gpio.h"
#include "i2c.h"

/* *
 * ENA - 
 * ENB - 
 * IN1 - 
 * IN2 - 
 * IN3 - 
 * IN4 - 
 *
 * ENA - 
 * ENB - 
 * IN1 - 
 * IN2 - 
 * IN3 - 
 * IN4 - 
 *
 * ENCODERA - PA 6,7 
 * ENCODERB - PA 0,1
 *
 * ENCODERA - PA 11,12 
 * ENCODERB - PC 6,7
 *
 * */

#define INL1 PC8
#define INL2 PC9
#define INL3 PC11
#define INL4 PC10
#define INR3 PC12
#define INR1 PB3
#define INR2 PA10
#define INR4 PB5

extern uint32_t flag_register;
extern uint8_t i2c_rx_buffer[I2C_BUFFER_SIZE];

void input_timer_init(void);

void output_timer_init(void);

void set_speed(uint16_t target);

void steps(int16_t target, int16_t dir);

void motor_wrapper(void* args);

#endif // !MOTORS_H
