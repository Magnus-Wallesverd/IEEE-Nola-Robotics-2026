#include "semaphore.h"

sem_t parser_sem;
sem_t* parser_sem_p = &parser_sem;

void sem_init(sem_t* s, void* item, int init_flag){
    s->flag = init_flag;
    s->item = item;
    
}

void signal(sem_t* s){
    s->flag++;
}

uint32_t waits(sem_t* s){
    if(s->flag > 0){
        s->flag--;
        return 1;
    }
    return 0;
}
