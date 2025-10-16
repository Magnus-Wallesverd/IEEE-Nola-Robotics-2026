// TODO figure out parameters

#ifndef SPI_H
#define SPI_H

#include <stdint.h>

typedef struct{
    volatile uint32_t CR1;      // 0x00
    volatile uint32_t CR2;      // 0x04
    volatile uint32_t SR;       // 0x08
    volatile uint32_t DR;       // 0x0C
    volatile uint32_t CRCPR;    // 0x10
    volatile uint32_t RXCRCR;   // 0x14
    volatile uint32_t TXCRCR;   // 0x18
    volatile uint32_t I2SCFGR;  // 0x1C
    volatile uint32_t I2SPR;    // 0x20
}SPI_TypeDef;

#define SPI1 ((SPI_TypeDef *) 0x40013000)
#define SPI2 ((SPI_TypeDef *) 0x40003800)
#define SPI3 ((SPI_TypeDef *) 0x40003C00)
#define SPI4 ((SPI_TypeDef *) 0x40013C00)

#define SPI_SSI  (1<<8)
#define SPI_TXE  (1<<1)
#define SPI_RXNE (1<<0)

// init spi 
void spi_init(SPI_TypeDef* SPIx, uint8_t ssm, uint16_t baud, uint8_t master, uint8_t cpol, uint8_t cpha);

// reset spi 
void spi_reset(SPI_TypeDef* SPIx);

void send_receive_wrapper(void* args);

#endif // !SPI_H
