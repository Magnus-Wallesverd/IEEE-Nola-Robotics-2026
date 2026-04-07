#include <stdint.h>
#include "dma.h"
#include "lock.h"
#include "tcb.h"

uint32_t dma_error_cnt = 0;
TCB* DMA_SPI_TX_tcb;

void DMA1_CH2_IRQHandler(void){

}

void DMA1_CH3_IRQHandler(void){
    if(DMA1_Status_Reg->ISR & TCIF3){
        unblock(DMA_SPI_TX_tcb);
    }else{
        dma_error_cnt++;       
    }
    // should unblock here
    DMA1_Status_Reg->IFCR |= GIF3;
    DMA1_CH3->CCR &= CCR_OFF;
}

void DMA1_Init(void){
    RCC->AHBENR |= DMA1_EN;
    //NVIC_ISERn |= ;
    //NVIC_IPRn |= ;
}

void configure_dma_spi(SPI_TypeDef* SPIx){


    switch((uint32_t)SPIx){
        case (uint32_t)SPI1:

            DMA1_Init();
            NVIC->ISER0 |= 1<<12;
            NVIC->ISER0 |= 1<<13;
            
            DMA1_CH2->CPAR |= (uint32_t)&SPI1->DR;
            DMA1_CH3->CPAR |= (uint32_t)&SPI1->DR;

            DMA1_CH2->CCR  |= DIR_Per2Mem;
            DMA1_CH3->CCR  |= DIR_Mem2Per;

            DMA1_CH2->CCR  |= DMA_TEIE|DMA_TCIE;       // Rx
            DMA1_CH3->CCR  |= DMA_TEIE|DMA_TCIE;       // Tx

            break;
        case (uint32_t)SPI2:
            break;
        case (uint32_t)SPI3:
            break;
        case (uint32_t)SPI4:
            break;
    }
}

void DMA_Transfer(DMA_TypeDef* CH, uint16_t msize, uint16_t psize, uint8_t minc, uint16_t len, uint32_t* maddr){

    CH->CCR  &= TRANSFER_MASK; 
    CH->CCR  |= msize|psize|minc;

    CH->CNDTR = len;
    
    CH->CMAR  = (uint32_t)maddr;

    CH->CCR |= CCR_EN;
}
