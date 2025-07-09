#ifndef DISPLAY_QUEUE_H
#define DISPLAY_QUEUE_H

#include "stm32f303.h"
#include <stdint.h>

// struct for "lcd items". 
// lcd items contain function pointers w/ args

typdef struct{
    uint32_t encoder_data;
    uint32_t pwm_data;
    uint32_t error_data;
}lcd_motor_data_t;

typedef struct {
    uint32_t tag;
    void*    data_ptr;  
    uint32_t timestamp;
}lcd_item_t;

// display queue function
lcd_item_t* dequeue(void);

// display queue function
void enqueue(lcd_item_t* payload);

// init
void displayinit(void);

extern uint32_t lock(void);
extern void unlock(void);
extern void yield(void);

#endif // !DISPLAY_QUEUE_H

