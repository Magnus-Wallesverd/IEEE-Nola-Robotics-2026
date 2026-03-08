#include "usart.h"

// use for the active usart peripheral 
// holds the pointer to peripheral and buffers
usart_t usart;

uint8_t tx_i = 0;
uint8_t rx_i = 0;

uint8_t usart_tx_buffer[USART_BUF_SIZE];
uint8_t usart_rx_buffer[USART_BUF_SIZE];
uint8_t data[USART_BUF_SIZE];

/*
 * TODO add error interrupts 
 */
void USART1_IRQHandler(void){
    
    if(USART1->ISR & USART_FE){
        USART1->ICR |= USART_FECF;
        usart.rx_buffer_p[0] = 0;
        usart.rx_buffer_p[1] = 0;
    }
    
    if((USART1->ISR & USART_RXNE) && (USART1->CR1 & USART_RXNEIE)){
        usart.rx_buffer_p[rx_i++%USART_BUF_SIZE] = usart.USARTx->RDR;
        int8_t theta = usart_rx_buffer[0];
        target = usart_rx_buffer[1];
        target_h = (*get_i2c_buffer()|*(get_i2c_buffer()+1)<<8) + (theta*16);
    }

    if(USART1->ISR & USART_ORE){
        USART1->ICR |= USART_ORECF;
        usart.rx_buffer_p[0]=0;
        usart.rx_buffer_p[1] = 0;
    }

    if((USART1->ISR & USART_TXE) && (USART1->CR1 & USART_TXEIE)){
        usart.USARTx->TDR = usart.tx_buffer_p[tx_i++];
        if(tx_i == USART_BUF_SIZE){
            tx_i = 0;
            USART1->CR1 &= ~USART_TXEIE;
            USART1->CR1 |=  USART_TCIE;
        } 
    }

    if((USART1->ISR & USART_TC) && (USART1->CR1 & USART_TCIE)){
            USART1->CR1 &=  ~USART_TCIE;
    }
}


// init usart clocks, the physical gpio, set baud
void usart_init(USART_Typedef* USARTx, GPIO_TypeDef* port, uint32_t pins, uint32_t baud){
    switch((uint32_t)USARTx){
        case (uint32_t)USART1:
            RCC->APB2ENR |= USART1_EN;
            NVIC->ISER1  |= NVIC_USART1;
            SetPinAlternate(port, pins);
            AlternateFunctionSet(port,pins,7);
            SetOutputSpeed(port, pins, 1);
            break;
        case (uint32_t)USART2:
            RCC->APB1ENR |= USART2_EN;
            NVIC->ISER1  |= NVIC_USART2;
            SetOutputSpeed(port, pins, 1);
            break;
        case (uint32_t)USART3:
            RCC->APB1ENR |= USART3_EN;
            NVIC->ISER1  |= NVIC_USART3;
            SetOutputSpeed(port, pins, 1);
            break;
        case(uint32_t) UART4:
            RCC->APB1ENR |= UART4_EN;
            NVIC->ISER1  |= NVIC_UART4;
            SetOutputSpeed(port, pins, 1);
            break;
        case(uint32_t) UART5:
            RCC->APB1ENR |= UART5_EN;
            NVIC->ISER1  |= NVIC_UART5;
            SetOutputSpeed(port, pins, 1);
            break;
    }

    usart.USARTx = USARTx;
    usart.tx_buffer_p = &usart_tx_buffer[0];
    usart.rx_buffer_p = &usart_rx_buffer[0];

    USARTx->CR1 |= CR1_SETUP;
    USARTx->BRR = CLK_64Mhz/baud;

    USARTx->CR3 |= (1<<12);
}

void load_tx(void){
    char db[] = "01234567"; 
    for(int i = 0; i < USART_BUF_SIZE; i++){
        data[i] = db[i];
    }
    usart.tx_buffer_p = data; 
}

void usart_begin(void* args){
    (void) args;
    usart.USARTx->CR1 |= USART_RXNEIE;
    // usart.USARTx->CR1 |= USART_TXEIE;
}

usart_t* get_usart_t(void){
    return &usart;
}
