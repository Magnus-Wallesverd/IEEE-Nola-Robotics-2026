#ifndef MOTORS_H
#define MOTORS_H

#include <stdint.h>
#include "gpio.h"
#include "i2c.h"
#include "usart.h"

#define INL1 PC8
#define INL2 PC9
#define INL3 PC11
#define INL4 PC10
#define INR3 PC12
#define INR1 PB3
#define INR2 PA10
#define INR4 PB5

#define DATA_BUF_SIZE 64

extern uint8_t i2c_rx_buffer[I2C_BUFFER_SIZE];
extern uint32_t* motor_data_i_p;
extern uint8_t motor_data[DATA_BUF_SIZE];

typedef struct{
    
} movement_t;

uint8_t* get_meas_pair(void);

void input_timer_init(void);

void output_timer_init(void);

void set_speed(uint16_t target);

void steps(int8_t target_cm, int16_t target_h);

void motor_wrapper(void* args);

void init_servo_TIM(void);

void servo(void* args);
#endif // !MOTORS_H
