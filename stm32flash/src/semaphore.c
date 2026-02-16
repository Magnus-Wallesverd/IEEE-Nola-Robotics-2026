#include "semaphore.h"

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
