#include "stm32f303.h"

node_t heap[DLL_SIZE];
uint32_t memory_bmp1 = 0;
uint32_t memory_bmp2 = 0;

void add_next(void* head, void* ptr){

}

void rm_next(void* head, void* ptr){

}

void add_prev(void* head, void* ptr){

}

void rm_prev(void* head, void* ptr){

}

void* malloc(void* ptr){
    if(~memory_bmp1 != 0){
        
    }
    return ptr;   
}

void free(uint32_t bmp){
    
}
