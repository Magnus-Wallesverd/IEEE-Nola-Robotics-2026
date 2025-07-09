#include "stm32f303.h"
#include <stdint.h>


lcd_item_t display_pool[POOL_SIZE];

// TODO 
// __attribute__((section(".display_queue")))
lcd_item_t* display_queue[SIZE];

lcd_item_t** front = &display_queue[0];
lcd_item_t** end   = &display_queue[0];


void enqueue(lcd_item_t* display){
    if(display_queue_count == SIZE){
        return;
    }else if(end <= &display_queue[SIZE-1]){
        *end = display;
        end++;
        display_queue_count++;
    } else {
        end = &display_queue[0];
        *end = display;
        end++;
        display_queue_count++;
    }
}

lcd_item_t* dequeue(void){
    // item gets set first
    lcd_item_t* item = *front;
    if(display_queue_count == 0){
        return (void*)0;
    }else if(front<=&display_queue[SIZE-1]){
        *front = (void*)0;
        front++;
        display_queue_count--;
        return item;
    } else {
        front = &display_queue[0];
        *front = (void*)0;
        front++;
        display_queue_count--;
        return item;
    }
}

// initializes displays into lcd items into a lcd pool
void displayinit(void){
    
    // displays table
    display_func_t function_table[POOL_SIZE] = {
        blink_led,
        blink_led,
    };
    
    // load a function pointers into a lcd item array
    for(int i = 0; i < POOL_SIZE; i++){
        display_pool[i].function = function_table[i];
        display_pool[i].args = (void*)0;
    }
    
    // need to enqueue the address of the display
    for(int i = 0; i < TASK_QUEUE_SIZE; i++){
        enqueue(&display_pool[i]);
    }
}
