#ifndef DISPLAY_QUEUE_H
#define DISPLAY_QUEUE_H

#include "stm32f303.h"
#include <stdint.h>

// struct for "lcd items". 
// lcd items contain function pointers w/ args
typedef struct {
    void (*function)(void* args);
    void* args;
}lcd_item_t;

// function pointer
typedef void (*display_func_t)(void* args);

// display queue function
lcd_item_t* dequeue(void);

// display queue function
void enqueue(lcd_item_t* display);

// init
void displayinit(void);

extern uint32_t lock(void);
extern void unlock(void);
extern void yield(void);

#endif // !DISPLAY_QUEUE_H

