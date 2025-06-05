 /*
 * This is the main function for the develop branch
 * It will be left blank so any person wanting to branch from develop
 * Any main.c programs made on different branches should be left in its own repo
 * 
 * Again... Do not pull request to develop with a main.c that has changes until agreed on
 * */

#include "stm32f303.h"


void main(void){
    /* Reset the RCC clock configuration to the default reset state ------------*/
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;

    /* Reset CFGR register */
    RCC->CFGR &= 0xF87FC00C;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE bits */
    RCC->CFGR &= (uint32_t)0xFF80FFFF;

    /* Reset PREDIV1[3:0] bits */
    RCC->CFGR2 &= (uint32_t)0xFFFFFFF0;

    /* Reset USARTSW[1:0], I2CSW and TIMs bits */
    RCC->CFGR3 &= (uint32_t)0xFF00FCCC;

    /* Disable all interrupts */
    RCC->CIR = 0x00000000;
    // for(volatile int i = 0; i < 10000; i++);
    
    RCC->AHBENR |= 0x20000;
     /* RCC->APB1ENR |= 1;
     */
    SetPinOutput(GPIOA,0xffff);
    /*AlternateFunctionSet(GPIOA,0x03,1);
    InitGenTIM(TIM2);
    */
    PinWrite(GPIOA,0xffff);
    while(1){
        for(volatile int i = 0; i < 10000; i++);
    }
 }
