#include "parser.h"

void parse_array(void* args){
    parser_t* parser = (parser_t*)args;
    uint8_t buffer_size = parser->buffer_size;
    uint8_t frame_size = parser->frame_size;
    uint32_t* dst_i = (parser->dst_i);
    uint32_t src_i = *(parser->src_i);
    uint8_t f_ID = parser->ID;
    
    if((src_i - *dst_i) < frame_size){
        signal(parser_sem_p);
        return;
    }
    while (*dst_i != src_i) {
        if(parser->src[(*dst_i)%buffer_size] != f_ID){
            (*dst_i)++;
        } else if(parser->src[(*dst_i)%buffer_size] == f_ID && parser->src[((*dst_i) + frame_size)%buffer_size] == f_ID){
            while(parser->src[((*dst_i)+1)%buffer_size] != f_ID){
                parser->dst[(*dst_i)%buffer_size] = parser->src[((*dst_i)+1)%buffer_size];
                (*dst_i)++;
            }
        } else {
            (*dst_i)++;
        }
    }
    signal(parser_sem_p);
}
