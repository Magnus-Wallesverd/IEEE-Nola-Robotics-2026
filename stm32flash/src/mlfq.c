#include "stm32f303.h"
#include <stdint.h>

#define NUM_OF_QUEUES 3
#define MLFQ_QUEUE_SIZE 10
#define HIGH_QUEUE_MAX_TIME 10
#define MID_QUEUE_MAX_TIME 20
#define LOW_QUEUE_MAX_TIME 50

queue_t high_queue;
queue_t mid_queue;
queue_t low_queue;

queue_t* high_queue_ptr = &high_queue;
queue_t* mid_queue_ptr  = &mid_queue;
queue_t* low_queue_ptr  = &low_queue;

void* high_queue_array[MLFQ_QUEUE_SIZE];
void* mid_queue_array[MLFQ_QUEUE_SIZE];
void* low_queue_array[MLFQ_QUEUE_SIZE];

void mlfq_init(void){

    queue_t* queue_ptr_array[NUM_OF_QUEUES] = {
        high_queue_ptr,
        mid_queue_ptr,
        low_queue_ptr
    };

    void** queue_array_list[NUM_OF_QUEUES] = {
        high_queue_array,
        mid_queue_array,
        low_queue_array
    };

    uint32_t queue_max_time[NUM_OF_QUEUES] = {
        HIGH_QUEUE_MAX_TIME,
        MID_QUEUE_MAX_TIME,
        LOW_QUEUE_MAX_TIME
    };

    for(int i = 0; i < NUM_OF_QUEUES; i++){
        queue_ptr_array[i]->array = queue_array_list[i];
        queue_ptr_array[i]->count = 0;
        queue_ptr_array[i]->size  = MLFQ_QUEUE_SIZE;
        queue_ptr_array[i]->max_time = queue_max_time[i];
        queue_ptr_array[i]->front = queue_array_list[i];
        queue_ptr_array[i]->end   = queue_array_list[i];
    }

    for(int i = 0; i < MLFQ_QUEUE_SIZE; i++){
        enqueue(high_queue_ptr,&_stcb[i]);
    }
}

void demote_thread(TCB* tcb){

    if(tcb->priority == HIGH){
        tcb->priority = MID;
        enqueue(mid_queue_ptr,dequeue(high_queue_ptr));
        tcb->queue_age = 0;
    } else if(tcb->priority == MID){
        tcb->priority = LOW;
        enqueue(low_queue_ptr,dequeue(mid_queue_ptr));
        tcb->queue_age = 0;
    } else { return; }
}
