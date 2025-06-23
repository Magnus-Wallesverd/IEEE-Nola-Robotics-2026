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

__attribute__((section(".task1"), used))  void  my_task1(void *ctx);
__attribute__((section(".task2"), used))  void  my_task2(void *ctx);
__attribute__((section(".task3"), used))  void  my_task3(void *ctx);
__attribute__((section(".task4"), used))  void  my_task4(void *ctx);
__attribute__((section(".task5"), used))  void  my_task5(void *ctx);
__attribute__((section(".task6"), used))  void  my_task6(void *ctx);
__attribute__((section(".task7"), used))  void  my_task7(void *ctx);
__attribute__((section(".task8"), used))  void  my_task8(void *ctx);
__attribute__((section(".task9"), used))  void  my_task9(void *ctx);
__attribute__((section(".task10"), used)) void  my_task10(void *ctx);

#endif // !TCB_H
