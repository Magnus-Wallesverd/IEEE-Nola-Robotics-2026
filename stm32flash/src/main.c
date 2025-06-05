 /*
 * This is the main function for the develop branch
 * It will be left blank so any person wanting to branch from develop
 * Any main.c programs made on different branches should be left in its own repo
 * 
 * Again... Do not pull request to develop with a main.c that has changes until agreed on
 * */

#include "stm32f303.h"


void main(void){

    for(volatile int i = 0; i < 10000; i++);
    /*RCC->AHBENR |= 0x20000;
     * RCC->APB1ENR |= 1;
     */
    //SetPinOutput(GPIOA,0xffff);
    /*AlternateFunctionSet(GPIOA,0x03,1);
    InitGenTIM(TIM2);
    PinWrite(GPIOA,0xffff);
    */
    while(1){
        for(volatile int i = 0; i < 10000; i++);
    }
 }
