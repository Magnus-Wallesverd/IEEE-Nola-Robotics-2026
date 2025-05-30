#include "/home/ieee/baremetal/stm32flash/stm32f303.h"

//LED is  at PA5 

void main(void){

// sets pin 5 to output mode
GPIOA->MODER |= 0x400;

// output at pin 5 HIGH
GPIOA->ODR &= 0x20;

}
