#include "rcc.h"
#include "flash.h"

void set_clock_32Mhz(void){
    while(!(RCC->CR & RCC_HSIRDY));
    
    FLASH->ACR |= FLASH_ACR_LATENCY2;

    RCC->CFGR |= RCC_PLL_4x|RCC_PLLSRC_HSI|RCC_PPRE1_DIV4;

    RCC->CR |= RCC_PLLON;
    while(!(RCC->CR & RCC_PLLRDY));

    RCC->CFGR |= RCC_SW_PLL;
    while(!(RCC->CFGR & RCC_SWS_PLL));
}
