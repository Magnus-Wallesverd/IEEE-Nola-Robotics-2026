#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include "i2c.h"
#include "lcd.h"

#define TASK_QUEUE_SIZE 5 
#define TRANSPORT_QUEUE_SIZE 5 
#define FUNC_POOL_SIZE 4

typedef void (*func_t)(void*);
typedef int  (*transport_t)(void*);

typedef struct{
    uint32_t size;
    uint32_t count;
    uint32_t max_time;
    void**   front;
    void**   end;
    void**   array;
}queue_t;

typedef struct {
    func_t fn;
    void* args;
}work_item_t;

typedef struct {
    void* args;
    transport_t fn;
}transport_item_t;

extern queue_t* task_queue_ptr;
extern queue_t* ready_queue_ptr;
extern queue_t* transport_queue_ptr;
extern work_item_t producer_item;

void enqueue(queue_t* q, void* args);
void* dequeue(queue_t* q);

void task_queue_init(void);
void transport_queue_init(void);
void ready_queue_init(void);
void priority_queue_init(void);

#endif // !QUEUE_H
