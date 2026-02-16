#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>
#include "queue.h"
#include "tcb.h"
#include "semaphore.h"

extern TCB *current_tcb;
extern uint32_t lock(uint32_t* flag);
extern void unlock(uint32_t* flag);
extern void yield(void);
extern void yield_isr(void);

void block(void);

void unblock(TCB* tcb);

void kernel_unblock(sem_t* s);

void flag_wait(TCB* tcb);

void flag_post(TCB* tcb);

extern uint32_t kernel_unblock_counter;

#endif // !LOCK_H

