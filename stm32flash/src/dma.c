#include "dma.h"
#include "spi.h"
#include "rcc.h"
#include <stdint.h>

uint16_t tx_buffer[TX_BUFFER_SIZE];
uint16_t rx_buffer[RX_BUFFER_SIZE];

void enable_dma(void){
    RCC->AHBENR |= 1;
}

void configure_spi(SPI_TypeDef* SPIx){
    switch((uint32_t)SPIx){
        case (uint32_t)SPI1:
            DMA->CPAR2 |= (uint32_t)&SPI1->DR;
            DMA->CMAR2 |= (uint32_t)tx_buffer; // make a buffer for this somewhere
            DMA->CPAR3 |= (uint32_t)&SPI1->DR;
            DMA->CMAR3 |= (uint32_t)rx_buffer; // make a buffer for this somewhere
            DMA->CNDTR2 |= 4;
            DMA->CNDTR3 |= 4;
            DMA->CCR2  |= 0x0582;       // Rx
            DMA->CCR3  |= 0x0592;       // Tx
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

void dma_rx_toggle(void){
    DMA->CCR2 ^= 1;     
}

void dma_tx_toggle(void){
    DMA->CCR3 ^= 1;     
}
