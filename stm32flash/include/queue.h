#ifndef QUEUE_H
#define QUEUE_H

#include "stm32f303.h"
#include <stdint.h>

typedef void (*func_t)(void* args);

typedef struct{
    uint32_t size;
    uint32_t count;
    void**   front;
    void**   end;
    void**   array;
}queue_t;

typedef struct {
    func_t fn;
    void* args;
}work_item_t;

void enqueue(queue_t* q, void* args);

void* dequeue(queue_t* q);

extern uint32_t lock(void);
extern void unlock(void);
extern void yield(void);

#endif // !QUEUE_H
