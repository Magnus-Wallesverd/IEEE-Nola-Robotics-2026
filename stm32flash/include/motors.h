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

#define ENCODER_FILTER_WEIGHT 20
#define TOF_FILTER_WEIGHT 80

#define DATA_BUF_SIZE 64

extern int16_t delta_tof;

typedef struct{
    int16_t args;
    uint8_t speed;
}motor_payload;

void input_timer_init(void);

void output_timer_init(void);

void set_speed(uint16_t target);

void motor_wrapper(void* args);

int lateral_left(void* args);

int lateral_right(void* args);

void servo(void* args);

int step2(int16_t args, uint8_t speed);
int step3(void* args);

int rotate2(int16_t args);
int rotate3(void* args);

int relative_pos(int16_t x1 , int16_t y1);

void turn_off_motors(void);

void motor_handler(void* args);

void motor_init(void);


#endif // !MOTORS_H
