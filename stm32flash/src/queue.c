#include <stdint.h>
#define SIZE 10

uint32_t queue[SIZE];
uint32_t *front = &queue[0];
uint32_t *end = &queue[0];
uint32_t count = 0;

void push(uint32_t item){
        
    if(count == SIZE){
        return;
    }else if(end <= &queue[SIZE-1]){
        *end = item;
        end++;
        count++;
    } else {
        end = &queue[0];
        *end = item;
        end++;
        count++;
    }    
}

void pop(){
    if(count == 0){
        return;
    }else if(front<=&queue[SIZE]){
        *front = 0;
        front++;
        count--;
    } else {
        front = &queue[0];
        *front = 0;
        front++;
        count--;
    }
}

void print(){
   
    // checks of queue is empty before printing 
    if(front!=end){
        for(uint32_t *i = &queue[0]; i < &queue[SIZE]; i++){
            printf("%d ", *i);
        }
    } else { printf("Queue empty \n"); }
    printf("\n");
}

int main() {
    
    return 0;
} 
