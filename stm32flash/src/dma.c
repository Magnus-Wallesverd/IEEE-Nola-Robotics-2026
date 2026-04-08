#include <stdint.h>
#include "dma.h"
#include "lock.h"
#include "tcb.h"

// the IRQ needs to be generic!!! use a stuct homie

uint32_t dma_tx_cnt = 0;
uint32_t dma_rx_cnt = 0;
uint32_t dma_error_cnt = 0;
uint32_t rx_byte = 0xFFFFFFFF;
uint32_t tx_byte = 0xA5;

TCB* DMA_tcb_1;
TCB* DMA_tcb_2;

void DMA1_CH2_IRQHandler(void){
    if(DMA1_Status_Reg->ISR & TCIF2){ 
        dma_rx_cnt++;
        /*unblock(DMA_tcb_1);*/
    }else{
        dma_error_cnt++;       
    }
    // should unblock here
    DMA1_Status_Reg->IFCR |= GIF2;
    DMA1_CH2->CCR &= CCR_OFF;

}

void DMA1_CH3_IRQHandler(void){
    if(DMA1_Status_Reg->ISR & TCIF3){
        dma_tx_cnt++;
        /*unblock(DMA_tcb_2);*/
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
            
            DMA1_CH2->CPAR |= (uint32_t)&SPIx->DR;
            DMA1_CH3->CPAR |= (uint32_t)&SPIx->DR;

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

/*1. Configure SPI (once)*/
/*2. Configure DMA channel (once)*/
/*3. For each transfer:*/
/*    - set CMAR (buffer)*/
/*    - set CNDTR (length)*/
/*    - enable TXDMAEN*/
/*    - enable DMA channel*/
/*4. Transfer runs automatically*/

void DMA_Transfer(DMA_TypeDef* CH, uint16_t msize, uint16_t psize, uint8_t minc, uint16_t len, uint32_t* maddr){

    CH->CCR  &= TRANSFER_MASK; 
    CH->CCR  |= msize|psize|minc;

    CH->CNDTR = len;

    CH->CMAR  = (uint32_t)maddr;

    CH->CCR |= CCR_EN;
}

void DMA_TXRX_Transfer(DMA_TypeDef* CH_RX, DMA_TypeDef* CH_TX, uint16_t msize, uint16_t psize, uint8_t minc_rx, uint8_t minc_tx, uint16_t len, uint32_t* maddr_rx, uint32_t* maddr_tx){
    
    CH_RX->CCR  &= TRANSFER_MASK; 
    CH_RX->CCR  |= msize|psize|minc_rx;
    CH_RX->CNDTR = len;
    CH_RX->CMAR  = (uint32_t)maddr_rx;

    CH_TX->CCR  &= TRANSFER_MASK; 
    CH_TX->CCR  |= msize|psize|minc_tx;
    CH_TX->CNDTR = len;
    CH_TX->CMAR  = (uint32_t)maddr_tx;

    CH_RX->CCR |= CCR_EN;
    CH_TX->CCR |= CCR_EN;
}

void DMA_Wrapper(void* args){
    (void)args;

    DMA_TXRX_Transfer(DMA1_CH2, DMA1_CH3, MSIZE_B, PSIZE_B, MINC_OFF, MINC_OFF, 0xFFFF, &rx_byte, &tx_byte);
}
