#ifndef TASK_H
#define TASK_H
#include <stdint.h>

extern uint32_t _shareblock;
typedef struct {
    volatile uint32_t reg[64];
} task_array;

#define public ((task_array*)(&_shareblock))
void wait(uint32_t ticks);
void cpu_time(uint8_t time_ms);
#endif
