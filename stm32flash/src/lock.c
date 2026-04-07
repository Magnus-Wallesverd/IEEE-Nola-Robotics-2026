#include "lock.h"
#include "tcb.h"
#include "semaphore.h"
#include "queue.h"


void block(void){
    current_tcb->state = BLOCKED;
    yield();
}

//cant enqueue here
void unblock(TCB* tcb){
    tcb->flags++;
    kernel_unblock_counter++;
}

void flag_wait(TCB* tcb){
    tcb->flags--;
}

void flag_post(TCB* tcb){
    tcb->flags++;
}

void task_wait(uint32_t ticks){
    uint32_t t0 = get_global_tick();
    while(get_global_tick() < ticks + t0){
        yield();
    }
}

void idle_task(void* args){
    (void) args;
    while(1);
}
