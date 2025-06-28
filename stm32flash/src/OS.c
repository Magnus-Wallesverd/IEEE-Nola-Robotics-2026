#include "stm32f303.h"
#include <stdint.h>

extern uint32_t _task1_end;
TCB _stcb[SIZE];
TCB *current_tcb;
TCB *next_tcb;

// static StackFrame fakeframe[SIZE];

void tcbinit(void){
    
    // list of functions 
    void (*function_list[SIZE])(void *) = {
        my_task1,
        my_task2,
        my_task3,
        my_task4,
        my_task5,
        my_task6,
        my_task7,
        my_task8,
        my_task9,
        my_task10
    };

    for(int i = 0; i < SIZE; i++){
        
        // _stcb[i].sp = &(_task1_end)+ i*0x100 ;
        _stcb[i].context = 0;
        _stcb[i].function = function_list[i];
        _stcb[i].pid = i;
        _stcb[i].state = 0;
        _stcb[i].prio = 0xB;
        _stcb[i].flags = 0xC;
    }

    // systick counter enable
    STK->CTRL |= 0x1;
}

void my_task1(void *ctx){
    (void)ctx;
    while(1);
     
}

void my_task2(void *ctx){
    (void)ctx;
    while (1); 
}

void my_task3(void *ctx){
    (void)ctx;
    while (1); 
}

void my_task4(void *ctx){
    (void)ctx;
    while (1); 
}

void my_task5(void *ctx){
    (void)ctx;
    while (1); 
}

void my_task6(void *ctx){
    (void)ctx;
    while (1); 
}

void my_task7(void *ctx){
    (void)ctx;
    while (1); 
}

void my_task8(void *ctx){
    (void)ctx;
    while (1); 
}

void my_task9(void *ctx){
    (void)ctx;
    while (1); 
}

void my_task10(void *ctx){
    (void)ctx;
    while (1); 
}

TCB* taskscheduler(void){
    next_tcb++;
    if(next_tcb >= &_stcb[SIZE]){
        next_tcb = _stcb;
    }
    return next_tcb;
}
