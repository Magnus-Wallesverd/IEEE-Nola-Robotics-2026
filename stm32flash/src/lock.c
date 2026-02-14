#include "stm32f303.h"

void block(void){
    current_tcb->state = BLOCKED;
    yield();
}
