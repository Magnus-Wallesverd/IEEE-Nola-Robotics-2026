#include "stm32f303.h"
#include <stdint.h>

TCB _stcb[SIZE];
uint32_t _staskspace[SIZE][STACK_SIZE / sizeof(uint32_t)];

void initstack(void){
    int a = 0xFF;   
    void (*function_list[SIZE])(void *) = {
        my_function1,
        my_function2,
        // my_function3,
        // my_function4,
        // my_function5,
        // my_function6,
        // my_function7,
        // my_function8,
        // my_function9,
        // my_function10
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

void my_function1(void *ctx){
    (void)ctx;
    SetPinOutput(GPIOA, 0x20);
    PinWrite(GPIOA,0x20);
}

void my_function2(void *ctx){
    (void)ctx;
    lcdinit();
}

void taskscheduler(void){
    _stcb[0].function(_stcb[0].context);
    _stcb[1].function(_stcb[1].context);
}

