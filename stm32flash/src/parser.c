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
    lateral_right
};

void dispatcher_init(void){
    
    global_dispatch.sem = &dispatch_sem;
    sem_init(global_dispatch.sem, &dispatch_item,1);
    sem_init(&transport_sem, &transport_item,1);

    ((work_item_t*)global_dispatch.sem->item)->fn = transport_handler;
    ((work_item_t*)global_dispatch.sem->item)->args = (void*) 0 ;
}

void parser_dispatcher(parser_t* parser){

    global_dispatch.ID = parser->ID;

    enum codes function_code = parser->dst[1]; 

    ((transport_item_t*)transport_sem.item)->fn = dispatch_table[function_code];
    ((transport_item_t*)transport_sem.item)->args = &parser->dst[parser->frame_size-2];

    
    if(wait(global_dispatch.sem)){
        producer_function(global_dispatch.sem);
    }
    
    if(wait(&transport_sem)){
        transport_producer_function(&transport_sem);
    }
    
    signal(&transport_sem);
}
    
// {0xAA, 1, 2, 3}

void parse_array(void* args){

    parser_t* parser = (parser_t*)args;
    uint8_t buffer_size = parser->buffer_size;
    uint32_t frame_size = parser->frame_size;
    parser->dst = parser_buffer;
    uint32_t src_i = *(parser->src_i);
    uint8_t f_ID = parser->ID;
    
    if((src_i - p_dst_i) < (frame_size)){
        signal(parser_sem_p);
        return;
    }
    while (p_dst_i != src_i) {
        if(parser->src[p_dst_i%buffer_size] != f_ID){
            p_dst_i++;
        } else {
            for(uint32_t i = 0; i < frame_size; i++) {
                parser->dst[i] = parser->src[((p_dst_i + i)) % buffer_size];
            }
            parser_dispatcher(parser);
            p_dst_i+=frame_size;
        }
    }
    signal(parser_sem_p);
}
