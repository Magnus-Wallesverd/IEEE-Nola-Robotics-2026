#ifndef TCB_H
#define TCB_H

#include "stm32f303.h"
#include <stdint.h>

#define SIZE 10
#define STACK_SIZE 1024

typedef struct {
    uint32_t *sp;   // this should store the sp address? confirm 
    void (*function)(void *context);
    void *context
    uint8_t  pid;
    uint8_t  state;
    uint8_t  prio;
    uint8_t  flags;
}TCB;

typedef enum {
    TASK_INACTIVE = 0,
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SLEEPING
}TaskState;

TCB task_list[SIZE];
uint32_t task_stack[SIZE][STACK_SIZE / sizeof(uint32_t)];

#endif // !TCB_H
