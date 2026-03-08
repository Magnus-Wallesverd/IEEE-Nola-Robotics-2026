#include "parser.h"
#include <stdint.h>

void parse_array(void* args){
    parser_t* parser = (parser_t*)args;
    uint8_t buffer_size = parser->buffer_size;
    uint8_t frame_size = parser->frame_size;

    while (parser->dst_i != parser->src_i) {
        if(parser->src[*(parser->dst_i)%buffer_size] != parser->ID){
            (*(parser->dst_i))++;
        } else if(parser->src[(*(parser->dst_i) + frame_size)%buffer_size] == parser->ID){
            while(parser->src[*(parser->dst_i+1)%buffer_size] != parser->ID){
                parser->dst[*(parser->dst_i)%buffer_size] = parser->src[*(parser->dst_i)%buffer_size];
            }
        } else {
            (*(parser->dst_i))++;
        }
    }
}
