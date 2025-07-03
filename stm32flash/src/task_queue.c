#include "stm32f303.h"
#include <stdint.h>

#define QUEUE_SIZE 4

work_item_t *task_queue[SIZE];
work_item_t *front = &task_queue[0];
work_item_t *end   = &task_queue[0];
uint32_t task_queue_count  = 0;

void taskinit(void){
    for(int i = 0; i < QUEUE_SIZE; i++){
        
        enqueue();
    }
}

void enqueue(work_item_t *task){
    if(task_queue_count == SIZE){
        return;
    }else if(end <= &task_queue[SIZE-1]){
        *end = task;
        end++;
        task_queue_count++;
    } else {
        end = &task_queue[0];
        *end = task;
        end++;
        task_queue_count++;
    }
}

work_item_t* dequeue(){
    work_item_t *item = front;
    if(task_queue_count == 0){
        return item;
    }else if(front<=&task_queue[SIZE]){
        *front = (void*)0;
        front++;
        task_queue_count--;
        return item;
    } else {
        front = &task_queue[0];
        *front = 0;
        front++;
        task_queue_count--;
        return item;
    }
}
