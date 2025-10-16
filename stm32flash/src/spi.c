//TODO setup functions... cleaner

#include "spi.h"
#include "rcc.h"

void spi_enable(SPI_TypeDef* SPIx){
    SPIx->CR1 |= (1<<6);
}

void spi_reset(SPI_TypeDef* SPIx){
    switch((uint32_t)SPIx){
        case (uint32_t)SPI1:
            RCC->APB2RSTR |= (1 << 12);
            break;
        case (uint32_t)SPI2:
            RCC->APB1RSTR |= (1 << 14);
            break;
        case (uint32_t)SPI3:
            RCC->APB1RSTR |= (1 << 15);
            break;
        case (uint32_t)SPI4:
            RCC->APB2RSTR |= (1 << 15);
            break;
   }
}

// TODO make sure to reset the spi if config needs changes
void set_baud(SPI_TypeDef* SPIx, uint16_t division){
    switch(division){
        case 2:
            SPIx->CR1 |=(0<<3);
            break;
        case 4:
            SPIx->CR1 |=(1<<3);
            break;
        case 8:
            SPIx->CR1 |=(2<<3);
            break;
        case 16:
            SPIx->CR1 |=(3<<3);
            break;
        case 32:
            SPIx->CR1 |=(4<<3);
            break;
        case 64:
            SPIx->CR1 |=(5<<3);
            break;
        case 128:
            SPIx->CR1 |=(6<<3);
            break;
        case 256:
            SPIx->CR1 |=(7<<3);
            break;
    }
}

void enable_ssm(SPI_TypeDef* SPIx, uint8_t mode){
    if(mode == 1){
        SPIx->CR1 |= (3<<8);
    }
}

// select master/ slave config
// slave 0, master 1
// reset spi if mode change
void master_select(SPI_TypeDef* SPIx, uint8_t mode){
    if(mode == 1){
        SPIx->CR1 |=(1<<2);
    }
}

void cpol_select(SPI_TypeDef* SPIx, uint8_t mode){
    if(mode == 1){
        SPIx->CR1 |= (1<<1);
    }
}

void cpha_select(SPI_TypeDef* SPIx, uint8_t mode){
    if(mode == 1){
        SPIx->CR1 |= (1<<0);
    }
}

// sets buffer threshold to trigger RXNE event in status register
void fifo_threshold(SPI_TypeDef* SPIx){
    SPIx->CR2 |= (1<<12);
}

// init the spi
void spi_init(SPI_TypeDef* SPIx, uint8_t ssm, uint16_t baud, uint8_t master, uint8_t cpol, uint8_t cpha){
    switch((uint32_t)SPIx){
        case (uint32_t)SPI1:
            RCC->APB2ENR |= (1 << 12);
            break;
        case (uint32_t)SPI2:
            RCC->APB1ENR |= (1 << 14);
            break;
        case (uint32_t)SPI3:
            RCC->APB1ENR |= (1 << 15);
            break;
        case (uint32_t)SPI4:
            RCC->APB2ENR |= (1 << 15);
            break;

        enable_ssm(SPIx, ssm);
        fifo_threshold(SPIx);
        set_baud(SPIx, baud);
        master_select(SPIx, master);
        cpol_select(SPIx, cpol);
        cpha_select(SPIx, cpha);
        spi_enable(SPIx);
   }
}

void send_receive_byte(SPI_TypeDef* SPIx, uint8_t byte){
    SPIx->CR1 &= ~SPI_SSI;
    while(!(SPIx->SR & SPI_TXE));
    SPIx->DR = byte;
    while(!(SPIx->SR & ~SPI_SSI));
    SPIx->CR1 |= SPI_SSI;
}

void send_receive_wrapper(void* args){
    (void)args;
    send_receive_byte(SPI1, A3);
}
