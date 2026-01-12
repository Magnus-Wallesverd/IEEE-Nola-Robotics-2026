#include "usart.h"

// use for the active usart peripheral 
// holds the pointer to peripheral and buffers
usart_t usart;

static uint8_t tx_i = 0;
static uint8_t rx_i = 0;

uint8_t tx_buffer[USART_BUF_SIZE];
uint8_t rx_buffer[USART_BUF_SIZE];

/*
 * TODO add error interrupts 
 */
void USART1_IRQHandler(void){
    
    if((USART1->ISR & RXNE) && (USART1->CR1 & RXNEIE)){
        usart.rx_buffer[rx_i++%USART_BUF_SIZE] = usart_t.USARTx->RDR;
    }
    
    if((USART1->ISR & TXE) && (USART1->CR1 & TXEIE)){
        usart_t.USARTx->TDR = usart.tx_buffer[tx_i++];
        if(tx_i == USART_BUF_SIZE){
            tx_i = 0;
            USART1->CR1 &= ~TXEIE;
            USART1->CR1 |=  TCIE;
        } 
    }

    if((USART1->ISR & TC) && (USART1->CR1 & TCIE)){
            USART1->CR1 &=  ~TCIE;
    }
}


// init usart clocks, the physical gpio, set baud
void usart_init(USART_Typedef* USARTx, GPIOx port, uint32_t pins, uint16_t baud){
    switch(USARTx){
        case USART1:
            RCC->APB2ENR |= USART1_EN;
            NVIC->ISER1  |= NVIC_USART1;
            PinWrite(port, pins);
            break;
        case USART2:
            RCC->APB1ENR |= USART2_EN;
            NVIC->ISER1  |= NVIC_USART2;
            PinWrite(port, pins);
            break;
        case USART3:
            RCC->APB1ENR |= USART3_EN;
            NVIC->ISER1  |= NVIC_USART3;
            PinWrite(port, pins);
            break;
        case UART4:
            RCC->APB1ENR |= UART4_EN;
            NVIC->ISER1  |= NVIC_UART4;
            PinWrite(port, pins);
            break;
        case UART5:
            RCC->APB1ENR |= UART5_EN;
            NVIC->ISER1  |= NVIC_UART5;
            PinWrite(port, pins);
            break;
    }

    usart.USARTx = USARTx;
    usart.tx_buffer_p = &tx_buffer;
    usart.rx_buffer_p = &rx_buffer;

    USARTx->CR1 |= CR1_SETUP;
    USARTx->CR3 |= EIE;
    USARTx->BRR = FCLK/baud;

    usart.USARTx->CR1 |= UE;
    usart.USARTx->CR1 |= TE;
    usart.USARTx->CR1 |= RE;
}

void load_tx(void){
    uint8_t send[USART_BUF_SIZE];
    for(uint8_t i = 0; i < USART_BUF_SIZE; i++){
        // *(usartx.tx_buffer_p) = ;
    }
}

void usart_begin(void){

}



