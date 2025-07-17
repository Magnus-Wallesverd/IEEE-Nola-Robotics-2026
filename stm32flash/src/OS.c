#include "stm32f303.h"
#include <stdint.h>

uint32_t global_tick;
uint32_t  task_flag = 0;
TCB _stcb[SIZE];
TCB *current_tcb;
TCB *next_tcb;

void tcbinit(void){

    for(int i = 0; i < SIZE; i++){
        _stcb[i].total_age = 0;
        _stcb[i].queue_age = 0;
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
        unlock(&task_flag);
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
    while(1){
        worker_function();
    }
}

void my_thread2(void *ctx){
    (void)ctx;
    while(1){
        worker_function();
    }
}

void my_thread3(void *ctx){
    (void)ctx;
    while(1){
        worker_function(); 
    }
}

void my_thread4(void *ctx){
    (void)ctx;
    while(1){
        worker_function();
    }
}

void my_thread5(void *ctx){
    (void)ctx;
    while(1){
        worker_function();
    }
}

void my_thread6(void *ctx){
    (void)ctx;
    while(1){
        worker_function();
    }
}

void my_thread7(void *ctx){
    (void)ctx;
    while(1){
        worker_function();
    }
}

void my_thread8(void *ctx){
    (void)ctx;
    while(1){
        worker_function();
    }
}

void my_thread9(void *ctx){
    (void)ctx;
    while(1){
        worker_function();
    }
}

void my_thread10(void *ctx){
    (void)ctx;
    while(1){
        worker_function();
    }
}

TCB* threadscheduler(void){
    next_tcb++;
    if(next_tcb >= &_stcb[SIZE]){
        next_tcb = _stcb;
    }
    return next_tcb;
}
