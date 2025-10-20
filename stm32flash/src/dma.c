#include "dma.h" #include "spi.h"

void enable_dma(void){
    RCC->AHBENR |= 1;
}

void configure_spi(SPI_TypeDef* SPIx){
    switch((uint32_t)SPIx){
        case (uint32_t)SPI1:
            DMA->CPAR2 |= (uint32_t)&SPI1->DR;
            DMA->CMAR2 |= ; // make a buffer for this somewhere
            DMA->CPAR3 |= (uint32_t)&SPI1->DR;
            DMA->CMAR3 |= ; // make a buffer for this somewhere
            DMA->CNDTR2 |= 1;
            DMA->CNDTR3 |= 1;
            DMA->CCR2  |= 0x0A82;       // Rx
            DMA->CCR3  |= 0x0A92;       // Tx
            // dont forget enable disable
            break;
        case (uint32_t)SPI2:
            break;
        case (uint32_t)SPI3:
            break;
        case (uint32_t)SPI4:
            break;
    }
}
