#include <stdint.h>
#include "dma.h"
#include "rcc.h"

void DMA_Init(void){
    if(!(RCC->AHBENR & DMA1_EN)){
        RCC->AHBENR |= DMA1_EN;
    }
}
