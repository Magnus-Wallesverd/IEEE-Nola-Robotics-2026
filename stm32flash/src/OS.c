// TODO 
// you changed the workers, 
// you need to finish blocking and yielding.
// ready_q stuff and going from there to testing

#include "stm32f303.h"
#include <stdint.h>

uint32_t kernel_unblock_counter = 0;
uint32_t global_tick = 0;
uint32_t task_flag = 0;

TCB _stcb[TCB_ARRAY_SIZE];
TCB *current_tcb;
TCB *next_tcb;

static void* ready_array[TCB_ARRAY_SIZE];
static queue_t ready_q;

sem_t sem_blocked[TCB_ARRAY_SIZE];

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
    while(1){

        work_item_t* item;
        
        if(/*producer flag == 1*/){
            enqueue(task_queue_ptr,/*work_item*/);
        }

        // take item off queue
        if(lock(&task_flag) == 1){
            item = (work_item_t*)dequeue(task_queue_ptr);
            unlock(&task_flag);
        } else { yield(); }
        
        if(item == (void*)0){
            yield();
        } else {
            item->fn(item->args);
            yield();
        }
    }   
}

void kernel_tcb_unblock(TCB tcb[]){
    for(int i = 0; i < TCB_ARRAY_SIZE; i++){
        if(tcb[i].flags > 0){
            tcb[i].flags--;
            tcb[i].state = READY;
            enqueue(ready_queue_ptr, &tcb[i]);
            kernel_unblock_counter--;
        }
    }
}

TCB* threadscheduler(void){
    if(kernel_unblock_counter){
        kernel_tcb_unblock(_stcb);       
    }
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
        current_tcb->state = RUNNING;
        return current_tcb;
    }
}
