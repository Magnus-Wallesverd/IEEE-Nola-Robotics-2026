// TODO 
// you changed the workers, 
// you need to finish blocking and yielding.
// ready_q stuff and going from there to testing

#include "stm32f303.h"
#include <stdint.h>

uint32_t global_tick;
uint32_t  task_flag = 0;
TCB _stcb[SIZE];
TCB *current_tcb;
TCB *next_tcb;

static void* ready_array[SIZE];
static queue_t ready_q;

uint32_t get_global_tick(void){
    return global_tick;
}

void os_queue_init(void){
    ready_q.size  = SIZE;
    ready_q.count = 0;
    ready_q.array = ready_array;
    ready_q.front = ready_array;
    ready_q.end   = ready_array;
}

void tcbinit(void){

    for(int i = 0; i < SIZE; i++){
        /*_stcb[i].total_age = 0;*/
        /*_stcb[i].queue_age = 0;*/
        /*_stcb[i].pid = i;*/
        _stcb[i].state = READY;
        /*_stcb[i].priority = HIGH;*/
        /*_stcb[i].queue_time = HIGH_QUEUE_MAX_TIME;*/
        enqueue(&ready_q, _stcb[i]);
    }
}

void worker_function(void){
    
    // take item off queue if able
    while(1){
        while(lock(&task_flag) == 0){
            yield();
        }

        work_item_t* item = (work_item_t*)dequeue(task_queue_ptr);
        item->fn(item->args);
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
    next_tcb = (TCB*)dequeue(&ready_q);
    next_tcb->state = RUNNING;
    return next_tcb;
}

TCB* nts(void){
    current_tcb->queue_age++;
    current_tcb->total_age++;
    if(current_tcb->queue_age >= current_tcb->queue_time){
        demote_task(current_tcb);
    }

    next_tcb++;
    if(next_tcb >= &_stcb[SIZE]){
        next_tcb = _stcb;
    }
    return next_tcb;
}

void yield_helper(void){
    current_tcb->state = READY;
    enqueue(&ready_q, current_tcb);
}

void block_helper(void){
    current_tcb->state = BLOCKED;
}
