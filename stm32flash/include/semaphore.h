#ifndef SEM_H
#define SEM_H

#include <stdint.h>

typedef struct {
    uint32_t flag;
    void* item;
} sem_t;

// add external objects here
// probably the array of semaphores
// probably individual special instances

void sem_init(sem_t* s, void* item, int init_flag);

void signal(sem_t* s);

uint32_t wait(sem_t* s);

extern sem_t parser_sem;
extern sem_t* parser_sem_p;

#endif // !SEM_H

