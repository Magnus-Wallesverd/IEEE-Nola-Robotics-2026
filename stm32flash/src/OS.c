#include "stm32f303.h"
#include <stdint.h>

uint32_t global_tick;
uint32_t  task_flag = 0;
TCB _stcb[SIZE];
TCB *current_tcb;
TCB *next_tcb;

void tcbinit(void){

    // list of functions 
    void (*function_list[SIZE])(void *) = {
        my_thread1,
        my_thread2,
        my_thread3,
        my_thread4,
        my_thread5,
        my_thread6,
        my_thread7,
        my_thread8,
        my_thread9,
        my_thread10
    };

    for(int i = 0; i < SIZE; i++){
        _stcb[i].context = 0;
        _stcb[i].function = function_list[i];
        _stcb[i].pid = i;
        _stcb[i].state = READY;
        _stcb[i].prio = 0xB;
        _stcb[i].flags = 0xC;
    }
}

void worker_function(void){
    
    work_item_t* item;
    
    // take item off queue
    current_tcb->state = RUNNING;
    if(lock(&task_flag) == 1){
        item = (work_item_t*)dequeue(task_queue_ptr);
        unlock();
    } else { yield(); }
    
    if(item == (void*)0){
        current_tcb->state = IDLE;
        yield();
        return;
    } else {
        item->fn(item->args);
        current_tcb->state = READY;
        yield();
    }

}

void my_thread1(void *ctx){
    (void)ctx;
    worker_function();
}

void my_thread2(void *ctx){
    (void)ctx;
    worker_function();
}

void my_thread3(void *ctx){
    (void)ctx;
    worker_function();
}

void my_thread4(void *ctx){
    (void)ctx;
    worker_function();
}

void my_thread5(void *ctx){
    (void)ctx;
    worker_function();
}

void my_thread6(void *ctx){
    (void)ctx;
    worker_function();
}

void my_thread7(void *ctx){
    (void)ctx;
    worker_function();
}

void my_thread8(void *ctx){
    (void)ctx;
    worker_function();
}

void my_thread9(void *ctx){
    (void)ctx;
    worker_function();
}

void my_thread10(void *ctx){
    (void)ctx;
    worker_function();
}

TCB* threadscheduler(void){
    next_tcb++;
    if(next_tcb >= &_stcb[SIZE]){
        next_tcb = _stcb;
    }
    return next_tcb;
}
