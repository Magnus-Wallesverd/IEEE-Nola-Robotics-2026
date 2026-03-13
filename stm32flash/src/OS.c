// TODO 
// you changed the workers, 
// you need to finish blocking and yielding.
// ready_q stuff and going from there to testing

#include "stm32f303.h"
#include <stdint.h>

work_item_t producer_item;

uint32_t kernel_unblock_counter = 0;
uint32_t global_tick = 0;
uint32_t task_flag = 0;
uint32_t transport_flag = 0;

TCB _stcb[TCB_ARRAY_SIZE];
TCB *current_tcb;
TCB *next_tcb;

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

void producer_function(sem_t* s){
    enqueue(task_queue_ptr,s->item);
}

void transport_producer_function(sem_t* s){
    enqueue(transport_queue_ptr,s->item);
}

void worker_function(void){
    while(1){

        work_item_t* consumer_item;
        
        // take item off queue
        if(lock(&task_flag) == 1){
            consumer_item = (work_item_t*)dequeue(task_queue_ptr);
            unlock(&task_flag);
        } else { yield(); }
        
        if(consumer_item == (void*)0){
            yield();
        } else {
            consumer_item->fn(consumer_item->args);
            yield();
        }
    }   
}

// add semaphore to gate scheduler overload

void transport_handler(void* args){
        (void) args;
        transport_item_t* transport_item;
        
        // take item off queue
        if(lock(&transport_flag) == 1){
            transport_item = (transport_item_t*)dequeue(transport_queue_ptr);
            unlock(&transport_flag);
        } else { yield(); }
        
        if(transport_item == (void*)0){
            return;
        } else {
        if(transport_item->fn(transport_item->args)){
            usart_load_tx(1);
        } else {
            usart_load_tx(0);
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
