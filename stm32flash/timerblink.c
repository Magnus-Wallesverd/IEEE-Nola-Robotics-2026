#include "stm32f303.h" 

void setup(void){

    TIM6->CR1 |= 0x85;

}

void main(void){
    

}
