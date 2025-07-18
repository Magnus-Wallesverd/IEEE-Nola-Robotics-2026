#ifndef TCB_H
#define TCB_H

#include "stm32f303.h"
#include <stdint.h>

#define SIZE 10
#define STACK_SIZE 1024
#define TASK_BLOCK (1024/sizeof(uint32_t))

typedef struct {
    uint32_t *sp;
    void (*function)(void *context);
    void *context;
    uint8_t  pid;
    uint8_t  state;
    uint8_t  prio;
    uint8_t  flags;
}TCB;

enum states{
    READY = 0,
    RUNNING,
    BLOCKED
};

extern TCB _stcb[SIZE];

__attribute__((section(".task1"), used, naked))  void  my_task1(void *ctx);
__attribute__((section(".task2"), used, naked))  void  my_task2(void *ctx);
__attribute__((section(".task3"), used, naked))  void  my_task3(void *ctx);
__attribute__((section(".task4"), used, naked))  void  my_task4(void *ctx);
__attribute__((section(".task5"), used, naked))  void  my_task5(void *ctx);
__attribute__((section(".task6"), used, naked))  void  my_task6(void *ctx);
__attribute__((section(".task7"), used, naked))  void  my_task7(void *ctx);
__attribute__((section(".task8"), used, naked))  void  my_task8(void *ctx);
__attribute__((section(".task9"), used, naked))  void  my_task9(void *ctx);
__attribute__((section(".task10"), used, naked)) void  my_task10(void *ctx);

#endif // !TCB_H
