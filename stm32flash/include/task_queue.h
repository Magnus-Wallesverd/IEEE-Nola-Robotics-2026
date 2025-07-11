#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include "stm32f303.h"
#include <stdint.h>

// struct for "work items". 
// work items contain function pointers w/ args
typedef struct {
    void (*function)(void* args);
    void* args;
}work_item_t;

// function pointer
typedef void (*task_func_t)(void* args);

// task queue function
work_item_t* dequeue(void);

// task queue function
void enqueue(work_item_t* task);

// init
void taskinit(void);

#endif // !TASK_QUEUE_H

