#include "stm32f303.h"
#include <stdint.h>

queue_t msg_queue;
queue_t* msg_queue_ptr = &msg_queue;
lcd_item_t msg_pool[MSG_QUEUE_SIZE];
void* msg_queue_array[MSG_QUEUE_SIZE];

func_t fn_table[POOL_SIZE] = {msg1,msg2,msg3};

void msg_queue_init(queue_t* q){
    q->count = 0;
    q->array = msg_queue_array;
    q->size  = MSG_QUEUE_SIZE;
    q->front = q->array;
    q->end   = q->array;

    // load a function pointers into a work item array
    for(int i = 0; i < FUNC_POOL_SIZE; i++){
        msg_pool[i].fn = fn_table[i];
        msg_pool[i].args = (void*)0;
    }
    
    // need to enqueue the address of the task
    for(int i = 0; i < TASK_QUEUE_SIZE; i++){
        enqueue(q, &msg_pool[i]);
    }
}
