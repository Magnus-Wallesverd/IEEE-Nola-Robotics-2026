#include "rcc.h"
#include "flash.h"

void set_clock_128Mhz(void){
    while(!(RCC->CR & RCC_HSIRDY));

    FLASH->ACR |= FLASH_ACR_LATENCY4;

    RCC->PLLCFGR |= RCC_PLLSRC_HSI|RCC_PLLCFGR_PLLN_8;
    // RCC->CFGR |= RCC_PPRE1_DIV8;

    RCC->CR |= RCC_PLLON;
    while(!(RCC->CR & RCC_PLLRDY));

    RCC->CFGR |= RCC_SW_PLL;
    while(!(RCC->CFGR & RCC_SWS_PLL));
}
