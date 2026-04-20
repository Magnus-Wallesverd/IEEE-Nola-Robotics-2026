#ifndef OS2_H
#define OS2_H
#include "lock.h"
#include <stdint.h>
#include "backend.h"
extern uint32_t _process1;
extern uint32_t _process2;
extern uint32_t _process3;
extern uint32_t _process4;
extern uint32_t _process5;

typedef struct {
    uint32_t reserve;
    uint32_t Ticks;
    uint16_t size_block;
    uint16_t size_tasks;
    uint32_t block[5];
    uint32_t priorBit;
    uint32_t reserve1;
    uint8_t readylist[32];
    uint8_t  r0;  //reserve r0 - r2 for select task
    uint8_t  r1;
    uint8_t  r2;
    uint8_t  r3;
    uint8_t  r4;
    uint8_t  r5;
    uint8_t  r6;
    uint8_t  set_prior;
    uint8_t  wait_q;

} Manager;
typedef struct { //should be 64 reg or 256 bytes for each task
     uint32_t sp;
     uint32_t status;
     uint32_t fn;
     uint32_t alloc;    //important reg
     uint32_t context[64];
     uint32_t data[60];

} threads;
extern threads* current_t;
extern threads* tasklist[];
#define mem ((Manager*)(&_process1 + 68))
#define task1 ((threads*)(&_process1))
#define task2 ((threads*)(&_process2))
#define task3 ((threads*)(&_process3))
#define task4 ((threads*)(&_process4))
#define task5 ((threads*)(&_process5))
void dht_rapper(void* args);
void q_up(void (*task_func)(void*));
void task_manager(void* args);
void toggle_led(void* args);
void wait(uint32_t ticks);
void unready(int k);
void ready(int k);
void set_priority(uint8_t num);
void bmp_size_up(void* args);
#endif
