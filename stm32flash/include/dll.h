#ifndef DLL_H
#define DLL_H
#include "stm32f303.h"

#define DLL_SIZE 64
/*
 * doubly linked list
 * add adds a node_t
 * rm removes a node_t
 * considering making malloc its own thing
 */

typedef struct{
    void* next;
    void* prev;
    void* data;
    void* reserved;
}node_t;

// finds empty block
extern uint32_t bmp_search(uint32_t);

void* init_head(void);

void add_next(void* head, void* ptr);
void rm_next(void* head, void* ptr);
void add_prev(void* head, void* ptr);
void rm_prev(void* head, void* ptr);

void* malloc(void* ptr);
void free(uint32_t bmp);

#endif // DLL_H
