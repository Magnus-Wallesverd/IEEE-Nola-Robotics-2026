#include "stm32f303.h"
#include <stdint.h>

TCB _stcb[SIZE];
uint32_t _staskspace[SIZE][STACK_SIZE / sizeof(uint32_t)];

void initstack(void){
    
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
        _stcb[i].sp = (uint32_t *)_staskspace[SIZE] - (i * TASK_BLOCK);
        _stcb[i].context = 0;
        _stcb[i].function = function_list[i];
        _stcb[i].pid = i;
        _stcb[i].state = 0xA;
        _stcb[i].prio = 0xB;
        _stcb[i].flags = 0xC;
    }
}

void my_task1(void *ctx){
    (void)ctx;
    SetPinOutput(GPIOA, 0x20);
    PinWrite(GPIOA, 0x20);
}

void my_task2(void *ctx){
    (void)ctx;
    lcdinit();
}

void my_task3(void *ctx){
    (void)ctx;
}

void my_task4(void *ctx){
    (void)ctx;
}
void my_task5(void *ctx){
    (void)ctx;
}

void my_task6(void *ctx){
    (void)ctx;
}

void my_task7(void *ctx){
    (void)ctx;
}

void my_task8(void *ctx){
    (void)ctx;
}

void my_task9(void *ctx){
    (void)ctx;
}

void my_task10(void *ctx){
    (void)ctx;
}

void taskscheduler(void){
    _stcb[0].function(_stcb[0].context);
    context_switch(&_stcb[0]);
    /*_stcb[1].function(_stcb[1].context);*/
    /*context_switch(&_stcb[0]);*/
}
