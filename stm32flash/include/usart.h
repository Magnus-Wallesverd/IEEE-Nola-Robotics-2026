#ifndef USART_H
#define USART_H

#include <stdint.h>
#include "gpio.h"
#include "rcc.h"
#include "backend.h"
#include "parser.h"
#include "semaphore.h"
#include "queue.h"
#include "tcb.h"
#include "motors.h"

#define UE      (1<<0)
#define RE      (1<<2)
#define TE      (1<<3)
#define USART_IDLEIE  (1<<4)
#define USART_RXNEIE   (1<<5)
#define USART_TCIE    (1<<6)
#define USART_TXEIE   (1<<7)
#define USART_RTOIE   (1<<26)
#define USART_FE      (1<<1)
#define USART_FECF     (1<<1)
#define USART_ORE  (1<<3)
#define USART_ORECF (1<<3)

#define EIE     (1<<0)

#define RTOEN   (1<<23)

#define USART_IDLE    (1<<4)
#define USART_RXNE    (1<<5)
#define USART_TC      (1<<6)
#define USART_TXE     (1<<7)

#define RATE 115200
#define USART_BRR FCLK/RATE
#define USART_TX_BUF_SIZE 4
#define USART_RX_BUF_SIZE 64

#define USART_HEADER 0xAA
#define USART_FOOTER 0x55
#define USART_FRAME_SIZE 4

#define CR1_SETUP UE|TE|RE

enum usart_state{
    USART_INACTIVE=0,
    USART_ACTIVE
};

typedef struct{
    volatile uint32_t CR1;    //0x00
    volatile uint32_t CR2;    //0x04
    volatile uint32_t CR3;    //0x08
    volatile uint32_t BRR;    //0x0C
    volatile uint32_t GTPR;   //0x10
    volatile uint32_t RTOR;   //0x14
    volatile uint32_t RQR;    //0x18
    volatile uint32_t ISR;    //0x1C
    volatile uint32_t ICR;    //0x20
    volatile uint32_t RDR;    //0x24
    volatile uint32_t TDR;    //0x28
} USART_Typedef;                    

typedef struct{
    USART_Typedef* USARTx;
    uint8_t* tx_buffer_p;
    uint8_t* rx_buffer_p;
    parser_t* parser;
    sem_t* sem;
    dispatcher_t* dispatch;
} usart_t;

#define USART1 ((USART_Typedef *) 0x40013800)
#define USART2 ((USART_Typedef *) 0x40004400)
#define USART3 ((USART_Typedef *) 0x40004800)
#define UART4  ((USART_Typedef *) 0x40004C00)
#define UART5  ((USART_Typedef *) 0x40005000)

extern int16_t target_h;
extern int16_t target;

void usart_init(USART_Typedef* USARTx, GPIO_TypeDef* port, uint32_t pins, uint32_t baud);
void usart_load_tx(int status);
void usart_begin(void* args);
usart_t* get_usart_t(void);
uint8_t* get_usart_rx(void);


#endif // !USART_H
