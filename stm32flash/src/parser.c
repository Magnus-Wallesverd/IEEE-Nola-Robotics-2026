#include "parser.h"
#include "tcb.h"
#include "queue.h"
#include "motors.h"
#include <semaphore.h>

work_item_t dispatch_item;
sem_t dispatch_sem;

uint8_t parser_buffer[PARSER_BUFFER_SIZE];
uint32_t p_dst_i = 0;
// dispatcher_t global_dispatch;

const transport_t dispatch_table[] = {
    step,
    rotate,
    lateral_left,
    lateral_right
};

void parser_dispatcher(parser_t* parser){

    dispatcher_t local_dispatch;

    local_dispatch.sem = &dispatch_sem;
    sem_init(local_dispatch.sem, &dispatch_item,1);

    local_dispatch.src = parser->dst;
    local_dispatch.ID = parser->ID;

    enum codes function_code = local_dispatch.src[1]; 

    ((transport_item_t*)local_dispatch.sem->item)->fn = dispatch_table[function_code];
    ((transport_item_t*)local_dispatch.sem->item)->args = &local_dispatch.src[parser->frame_size-1];
    
    transport_producer_function(local_dispatch.sem);
    
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
