#include "parser.h"
#include "tcb.h"
#include "queue.h"
#include "motors.h"
#include <semaphore.h>

transport_item_t transport_item;

work_item_t dispatch_item;

sem_t dispatch_sem;
sem_t transport_sem;


uint8_t parser_buffer[PARSER_BUFFER_SIZE];
uint32_t p_dst_i = 0;

dispatcher_t global_dispatch;

const transport_t dispatch_table[] = {
    step,
    rotate,
    lateral_left,
    lateral_right,
};

void dispatcher_init(void){
    global_dispatch.sem = &dispatch_sem;
    sem_init(global_dispatch.sem, &dispatch_item,1);
    sem_init(&transport_sem, &transport_item,1);

    ((work_item_t*)global_dispatch.sem->item)->fn = transport_handler;
    ((work_item_t*)global_dispatch.sem->item)->args = (void*) 0 ;
}

void parser_dispatcher(void* args){
    (void) args;

    enum codes function_code = parser_buffer[1]; 

    ((transport_item_t*)transport_sem.item)->fn = dispatch_table[function_code];
    ((transport_item_t*)transport_sem.item)->args = &parser_buffer[2];

    
    if(wait(global_dispatch.sem)){
        producer_function(global_dispatch.sem);
    }
    
    if(wait(&transport_sem)){
        transport_producer_function(&transport_sem);
    }
    
    signal(global_dispatch.sem);
    signal(&transport_sem);
}
