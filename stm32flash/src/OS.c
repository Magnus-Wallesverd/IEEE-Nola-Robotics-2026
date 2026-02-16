#include "stm32f303.h"
#include <stdint.h>
#include "math.h"

uint32_t global_tick = 0;
uint32_t  task_flag = 0;
TCB _stcb[TCB_ARRAY_SIZE];
TCB *current_tcb;
TCB *next_tcb;
uint32_t kernel_idle_flag = 0;


uint32_t get_global_tick(void){
    return global_tick;
}

void tcbinit(void){

    for(int i = 0; i < TCB_ARRAY_SIZE; i++){
        _stcb[i].total_age = 0;
        _stcb[i].queue_age = 0;
        _stcb[i].pid = i;
        _stcb[i].state = READY;
        _stcb[i].prio = 0x0;
        _stcb[i].flags = 0x0;
    }
}

void worker_function(void){
    
    work_item_t* item;
    
    // take item off queue
    if(lock(&task_flag) == 1){
        item = (work_item_t*)dequeue(task_queue_ptr);
        unlock(&task_flag);
    } else { yield(); }
    
    if(item == (void*)0){
        yield();
        return;
    } else {
        item->fn(item->args);
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
    while(1){

        if(current_tcb->state == BLOCKED){
            next_tcb = (TCB*)dequeue(ready_queue_ptr);
            next_tcb->state = RUNNING;
            return next_tcb;
        } else if(ready_queue_ptr->count > 0){
            enqueue(ready_queue_ptr, current_tcb);
            current_tcb->state = READY;
            next_tcb = (TCB*)dequeue(ready_queue_ptr);
            next_tcb->state = RUNNING;
            return next_tcb;
        } else {
            return current_tcb;
        }
        if(next_tcb == 0){
            __asm volatile("BKPT #0"); // scheduler error

        }
    }
}

