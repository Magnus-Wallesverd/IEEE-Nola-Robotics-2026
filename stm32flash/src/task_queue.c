#include "stm32f303.h"
#include <stdint.h>

queue_t task_queue;
queue_t* task_queue_ptr = &task_queue;
work_item_t task_pool[TASK_QUEUE_SIZE];
void* task_queue_array[TASK_QUEUE_SIZE];

// should start thinking of easier ways to get functions in here
const func_t fn_table[FUNC_POOL_SIZE] = {
    send_receive_wrapper,
    send_receive_wrapper,
    send_receive_wrapper
};

void task_queue_init(void){
    task_queue_ptr->count = 0;
    task_queue_ptr->array = task_queue_array;
    task_queue_ptr->size  = TASK_QUEUE_SIZE;
    task_queue_ptr->front = task_queue_ptr->array;
    task_queue_ptr->end   = task_queue_ptr->array;

    // load a function pointers into a work item array
    for(int i = 0; i < FUNC_POOL_SIZE; i++){
        task_pool[i].fn = fn_table[i];
        task_pool[i].args = (void*)0;
    }
    
    // need to enqueue the address of the task
    for(int i = 0; i < TASK_QUEUE_SIZE; i++){
        enqueue(task_queue_ptr, &task_pool[i]);
    }
}
