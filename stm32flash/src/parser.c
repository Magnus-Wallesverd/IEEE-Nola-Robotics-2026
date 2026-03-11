#include "parser.h"
#include "tcb.h"

uint8_t parser_buffer[PARSER_BUFFER_SIZE];
uint32_t p_dst_i = 0;

// this function should know about a semaphore for neat scheduling
// if this function knows aout semaphores then it can call producer
// parse_array doesnt know about semaphores

// void parse

void parser_logic(parser_t* parser){

    
    // if(wait(parser->work_sem_p)){
    //     producer_function(parser->work_sem_p);
    // }
}

void parse_array(void* args){
    parser_t* parser = (parser_t*)args;
    uint8_t buffer_size = parser->buffer_size;
    uint32_t frame_size = parser->frame_size;
    uint32_t* dst_i_p = &p_dst_i;
    parser->dst = parser_buffer;
    uint32_t src_i = *(parser->src_i);
    uint8_t f_ID = parser->ID;
    uint32_t parser_i = 0;
    
    if((src_i - *dst_i_p) <= (frame_size)){
        signal(parser_sem_p);
        return;
    }
    while (*dst_i_p != src_i) {
        if(parser->src[(*dst_i_p)%buffer_size] != f_ID){
            (*dst_i_p)++;
        } else if(parser->src[((*dst_i_p) + frame_size)%buffer_size] == f_ID){
            for(uint32_t i = 0; i < frame_size; i++){
                parser->dst[parser_i % buffer_size] = parser->src[((*dst_i_p+parser_i))%buffer_size];
                parser_i++;
            }
            parser_i = 0;
            parser_logic(parser);
        } else {
           (*dst_i_p)++;
        }
    }
    signal(parser_sem_p);
}
