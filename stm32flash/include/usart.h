#ifndef USART_H
#define USART_H

#include <stdint.h>
#include "gpio.h"
#include "rcc.h"

#define UE      (1<<0)
#define RE      (1<<2)
#define TE      (1<<3)
#define IDLEIE  (1<<4)
#define RXNIE   (1<<5)
#define TCIE    (1<<6)
#define TXEIE   (1<<7)
#define RTOIE   (1<<26)

#define EIE     (1<<0)

#define RTOEN   (1<<23)

#define IDLE    (1<<4)
#define RXNE    (1<<5)
#define TC      (1<<6)
#define TXE     (1<<7)

#define RATE 115200
#define BRR FCLK/RATE
#define USART_BUF_SIZE 8

#define CR1_SETUP RTOIE|TXEIE|TCIE|RXNIE

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
    USART_typedef* USARTx;
    uint8_t* tx_buffer_p;
    uint8_t* rx_buffer_p;
} usart_t

#define USART1 ((USART_Typedef *) 0x40013800)
#define USART2 ((USART_Typedef *) 0x40004400)
#define USART3 ((USART_Typedef *) 0x40004800)
#define UART4  ((USART_Typedef *) 0x40004C00)
#define UART5  ((USART_Typedef *) 0x40005000)

void usart_init(USART_Typedef* USARTx, GPIO_TypeDef* port, uint32_t pins, uint32_t baud);
void load_tx(void);
void usart_begin(void* args);
usart_t* get_usart_t(void);

#endif // !USART_H
