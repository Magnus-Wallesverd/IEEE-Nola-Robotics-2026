#include "semaphore.h"

sem_t parser_sem;
sem_t* parser_sem_p = &parser_sem;

void sem_init(sem_t* s, void* item){
    s->flag = 0;
    s->item = item;
}

void signal(sem_t* s){
    s->flag++;
}

uint32_t wait(sem_t* s){
    if(s->flag > 0){
        s->flag--;
        return 1;
    }
    return 0;
}
