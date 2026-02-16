#include "stm32f303.h"
#include <stdint.h>

node_t heap[DLL_SIZE];
uint32_t memory_bmp1 = 0;
uint32_t memory_bmp2 = 0;
uint32_t* bmp1_ptr= &memory_bmp1;
uint32_t* bmp2_ptr= &memory_bmp2;

// create head node
void init_head(void){
    // search & return free node 
    heap[bmp_search(memory_bmp1)];


    
    return;
}

void add_next(void* head, void* ptr){
    if(~memory_bmp1 != 0){
        node_malloc(memory_bmp1, ptr);       
    } else if(~memory_bmp2 != 0){
        node_malloc(memory_bmp2, ptr);       
    } else { return; }

void rm_next(void* head, void* ptr){
    if(~memory_bmp1 != 0){

    } else if(~memory_bmp2 != 0){

    } else { return; }
}

void add_prev(void* head, void* ptr){
    if(~memory_bmp1 != 0){

    } else if(~memory_bmp2 != 0){

    } else { return; }
}

void rm_prev(void* head, void* ptr){
    if(~memory_bmp1 != 0){

    } else if(~memory_bmp2 != 0){

    } else { return; }

}

void* node_malloc(uint32_t bmp, void* ptr){
        // index
        return (void*)(ptr+bmp_search(bmp));
}

void free(uint32_t i, uint32_t* bmp_ptr){
    *bmp_ptr ^= (1 << i);
}
