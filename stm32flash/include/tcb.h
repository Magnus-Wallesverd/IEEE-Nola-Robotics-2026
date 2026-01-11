#ifndef TCB_H
#define TCB_H

#include "stm32f303.h"
#include <stdint.h>

#define SIZE 10
#define STACK_SIZE 1024
#define TASK_BLOCK (1024/sizeof(uint32_t))

// old tcb struct still in use 
// TODO redesign to save bytes
typedef struct {
    uint32_t *sp;
    uint32_t total_age;
    uint32_t queue_age;
    uint8_t  state;
    uint8_t  pid;
    uint8_t  priority;
    uint8_t  queue_time;
}TCB;

// states for threads
enum states{
    READY = 0,
    RUNNING,
    BLOCKED,
    IDLE
};

enum thread_priority{
    LOW = 0,
    MID,
    HIGH
};

__attribute__((section(".thread1"), used, naked))  void  my_thread1(void *ctx);
__attribute__((section(".thread2"), used, naked))  void  my_thread2(void *ctx);
__attribute__((section(".thread3"), used, naked))  void  my_thread3(void *ctx);
__attribute__((section(".thread4"), used, naked))  void  my_thread4(void *ctx);
__attribute__((section(".thread5"), used, naked))  void  my_thread5(void *ctx);
__attribute__((section(".thread6"), used, naked))  void  my_thread6(void *ctx);
__attribute__((section(".thread7"), used, naked))  void  my_thread7(void *ctx);
__attribute__((section(".thread8"), used, naked))  void  my_thread8(void *ctx);
__attribute__((section(".thread9"), used, naked))  void  my_thread9(void *ctx);
__attribute__((section(".thread10"), used, naked)) void  my_thread10(void *ctx);

extern TCB _stcb[SIZE];
extern void (*function_list[SIZE])(void *);

#endif // !TCB_H
