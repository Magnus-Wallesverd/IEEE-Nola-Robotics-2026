#include "stm32f303.h"
#include <stdint.h>

void enqueue(queue_t* q, void* args){
    if(q->count == q->size || *(uint32_t*)args == 0){
        return;
    }else if(q->end <= q->array+q->size-1){
        *(q->end) = args;
        q->end++;
        q->count++;
    } else {
        q->end = q->array;
        *(q->end) = args;
        q->end++;
        q->count++;
    }
}

void* dequeue(queue_t* q){
    void* args;
    if(q->count == 0){
        return (void*)0;
    }else if(q->front<=q->array+q->size-1){
        args = *(q->front);
        *(q->front) = (void*)0;
        q->front++;
        q->count--;
        return args;
    } else {
        q->front = q->array;
        args = *(q->front);
        *(q->front) = (void*)0;
        q->front++;
        q->count--;
        return args;
    }
}
