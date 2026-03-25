#include "usart.h"

// use for the active usart peripheral 
// holds the pointer to peripheral and buffers
usart_t usart;
parser_t usart_parser;

work_item_t usart_item;

int overun_flag;
int frame_error_flag;
int tx_counter = 0;

enum usart_state u_state_machine = USART_INACTIVE;

dispatcher_t usart_dispatcher;

sem_t usart_sem;

uint32_t usart_tx_i = 0;
uint32_t usart_rx_i = 0;

uint8_t package[] = {USART_HEADER,0,0,USART_FOOTER};
uint8_t usart_tx_buffer[USART_TX_BUF_SIZE];
uint8_t usart_rx_buffer[USART_RX_BUF_SIZE];
uint8_t data[USART_TX_BUF_SIZE];

/*
 * TODO add error interrupts 
 */
void USART1_IRQHandler(void){
    
    if(USART1->ISR & USART_FE){
        USART1->ICR |= USART_FECF;
        frame_error_flag++;
    }
    
    if((USART1->ISR & USART_RXNE) && (USART1->CR1 & USART_RXNEIE)){
        uint8_t data = USART1->RDR;
        if(data == USART_HEADER){
            u_state_machine = USART_ACTIVE;
        }
        switch(u_state_machine){
            case USART_INACTIVE:
                return;
            case USART_ACTIVE:
                parser_buffer[usart_rx_i++] = data;
                if(usart_rx_i == USART_FRAME_SIZE){
                    usart_rx_i = 0;
                    u_state_machine = USART_INACTIVE;
                    if(wait(usart.sem)){
                        producer_function(usart.sem);
                        signal(usart.sem);
                    }
                }
                return;
        }
    }

    if(USART1->ISR & USART_ORE){
        USART1->ICR |= USART_ORECF;
        overun_flag++;
    }

    if((USART1->ISR & USART_TXE) && (USART1->CR1 & USART_TXEIE)){
        usart.USARTx->TDR = usart.tx_buffer_p[usart_tx_i++];
        if(usart_tx_i == USART_TX_BUF_SIZE){
            usart_tx_i = 0;
            USART1->CR1 &= ~USART_TXEIE;
            USART1->CR1 |=  USART_TCIE;
        } 
    }

    if((USART1->ISR & USART_TC) && (USART1->CR1 & USART_TCIE)){
            USART1->CR1 &=  ~USART_TCIE;
            tx_counter++;
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
    usart.tx_buffer_p = package;
    usart.rx_buffer_p = &usart_rx_buffer[0];
    
    usart.sem = &usart_sem;

    sem_init(usart.sem,(void*)&usart_item,1);  

    usart.dispatch = &usart_dispatcher;
    // usart.dispatch->header = USART_HEADER;
    // usart.dispatch->footer = USART_FOOTER;

    ((work_item_t*)usart.sem->item)->fn = parser_dispatcher;

    USARTx->CR1 |= CR1_SETUP;
    USARTx->BRR = CLK_64Mhz/baud;

    USARTx->CR3 |= (1<<12);
}

void usart_load_tx(int status){

    package[1] = status;
    usart.USARTx->CR1 |= USART_TXEIE;
    
}

void usart_begin(void* args){
    (void) args;
    // usart.USARTx->CR1 |= USART_RXNEIE;
    usart.USARTx->CR1 |= USART_RXNEIE;
    usart_load_tx(1);
}

uint8_t* get_usart_rx(void){
    return usart_rx_buffer;
}

usart_t* get_usart_t(void){
    return &usart;
}
