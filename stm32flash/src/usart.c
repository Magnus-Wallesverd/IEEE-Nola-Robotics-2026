#include "usart.h"
#include "tcb.h"
#include "lock.h"

// use for the active usart peripheral 
// holds the pointer to peripheral and buffers
// Camera function IDs
#define FN_SEE_TAG  0x10
#define FN_ID       0x01
#define FN_START    0x09

#define MAG_THRESHOLD_SQ  10000

uint8_t telemetry_pad;

typedef enum {
    TX_LED = 0,
    RX_LED,
    FIND_TAG,
    RX_FIND,
    GET_ID_F,
    RX_ID_F,
    GET_ID_B,
    RX_ID_B
}usart_state_code;

transport_item_t cam_frame;

usart_payload led_start = { .f_ID = FN_START, .LSB = 0, .MSB =0x0 };
usart_payload find_tag = { .f_ID = FN_SEE_TAG, .LSB = 0, .MSB =0x0 };
usart_payload tag_id_f = { .f_ID = FN_ID, .LSB = 1, .MSB =0x0 };
usart_payload tag_id_b = { .f_ID = FN_ID, .LSB = 2, .MSB =0x0 };

usart_t usart;

work_item_t usart_item;

int overun_flag;
int frame_error_flag;
int rx_flag = 0;
int tx_counter = 0;
int rx_counter = 0;

enum usart_state u_state_machine = USART_INACTIVE;

sem_t usart_sema;

uint32_t usart_tx_i = 0;
uint32_t usart_rx_i = 0;

uint8_t package[] = {USART_HEADER,0,0,0};
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
                break;
            case USART_ACTIVE:
                usart_rx_buffer[usart_rx_i++%USART_RX_BUF_SIZE] = data;
                if(usart_rx_i % USART_FRAME_SIZE == 0){
                    rx_flag = 1;
                    // usart_rx_i = 0;
                    u_state_machine = USART_INACTIVE;
                    unblock(transport_tcb);
                    unblock(usart_state_tcb);
                }
                break;
        }
        rx_counter++;
    }

    if(USART1->ISR & USART_ORE){
        USART1->ICR |= USART_ORECF;
        overun_flag++;
    }

    if((USART1->ISR & USART_TXE) && (USART1->CR1 & USART_TXEIE)){
        usart.USARTx->TDR = usart.tx_buffer_p[usart_tx_i++];
        rx_flag = 0;
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
        case (uint32_t)USART2:;
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
    usart.rx_buffer_p = usart_rx_buffer;
    
    sem_init(&usart_sema,1);  

    USARTx->CR1 |= CR1_SETUP;
    USARTx->BRR = CLK_32Mhz/baud;

    USARTx->CR3 |= (1<<12);
    usart.USARTx->CR1 |= USART_RXNEIE;
}

void usart_load_tx(void* args){
    
    package[1] = ((usart_payload*)args)->f_ID; 
    package[2] = ((usart_payload*)args)->LSB; 
    package[3] = ((usart_payload*)args)->MSB;

    usart.USARTx->CR1 |= USART_TXEIE;

}

uint8_t* get_usart_rx(void){
    return usart.rx_buffer_p;
}

usart_t* get_usart_t(void){
    return &usart;
}

//transport_item_t usart_frame2;
//usart_payload usart_data2 = {
//    .f_ID = 1, .LSB = 0xB, .MSB =0xC  
//};
//void usart_test2(int x){
//    for(int i = 0; i < x; i++){
//        usart_frame2.fn = usart_load_tx; 
//        usart_frame2.args = (void*)&usart_data2;
//        enqueue(transport_queue_ptr, (void*)&usart_frame2);
//    }
//}

void usart_state_update(void* args){
    (void*) args;

    usart_state_tcb = current_tcb;
    cam_frame.fn = usart_load_tx; 
    static uint8_t usart_state = TX_LED;

    while(1){
        switch(usart_state){
            case TX_LED:
                cam_frame.args = (void*)&led_start;
                enqueue(transport_queue_ptr, (void*)&cam_frame);
                usart_state = RX_LED;
                block();
                break;
            case RX_LED: 
                if(rx_flag){
                    if(usart_rx_buffer[1]) usart_state = FIND_TAG;
                }  else {
                    usart_state = TX_LED;
                }
                break;

            case FIND_TAG:
                cam_frame.args = (void*)&find_tag;
                enqueue(transport_queue_ptr,(void*)&cam_frame);
                usart_state = RX_FIND;
                block();
                break;
            case RX_FIND:
                if(rx_flag){
                    if(usart_rx_buffer[1] & 1) {
                        cam_frame.args = (void*)&tag_id_f;
                        enqueue(transport_queue_ptr,(void*)&cam_frame);
                        usart_state = GET_ID_F;
                    } else if (usart_rx_buffer[1]&2){
                        cam_frame.args = (void*)&tag_id_b;
                        enqueue(transport_queue_ptr,(void*)&cam_frame);
                        usart_state = GET_ID_B;
                    }
                } else {
                    usart_state = FIND_TAG;
                }
                break;

            case GET_ID_F:
                cam_frame.args = (void*)&tag_id_f;
                enqueue(transport_queue_ptr,(void*)&cam_frame);
                usart_state = RX_ID_F;
                break;
            case RX_ID_F:
                if(rx_flag){
                    if (usart_rx_buffer[1] <= 4){
                        telemetry_pad = usart_rx_buffer[1];
                    }
                } else {
                    usart_state = FIND_TAG;
                }
                break;

            case GET_ID_B:
                cam_frame.args = (void*)&tag_id_b;
                enqueue(transport_queue_ptr,(void*)&cam_frame);
                usart_state = RX_ID_B;
                break;
            case RX_ID_B:
                if(rx_flag){
                    if (usart_rx_buffer[1] <= 4){
                        telemetry_pad = usart_rx_buffer[1];
                    }
                } else {
                    usart_state = FIND_TAG;
                }
                break;
            }
    }
}
