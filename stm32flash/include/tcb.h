#ifndef TCB_H
#define TCB_H

#include "stm32f303.h"
#include <stdint.h>

#define SIZE 10
#define STACK_SIZE 1024
#define TASK_BLOCK (1024/sizeof(uint32_t))

typedef struct {
    uint32_t *sp;   // this should store the sp address? confirm 
    void (*function)(void *context);
    void *context;
    uint8_t  pid;
    uint8_t  state;
    uint8_t  prio;
    uint8_t  flags;
}TCB;

extern TCB _stcb[SIZE];

extern uint32_t _staskspace[SIZE][STACK_SIZE / sizeof(uint32_t)];

extern void save_sp(TCB *_stcb);

void my_function1(void *ctx);
void my_function2(void *ctx);
void my_function3(void *ctx);
void my_function4(void *ctx);
void my_function5(void *ctx);
void my_function6(void *ctx);
void my_function7(void *ctx);
void my_function8(void *ctx);
void my_function9(void *ctx);
void my_function10(void *ctx);

#endif // !TCB_H
