#ifndef DMA_H
#define DMA_H

#include "spi.h"
#include "rcc.h"

#include <stdint.h>

#define GIF1  1<<0 
#define TCIF1 1<<1 
#define HTIF1 1<<2 
#define TEIF1 1<<3 
#define GIF2  1<<4 
#define TCIF2 1<<5 
#define HTIF2 1<<6 
#define TEIF2 1<<7 
#define GIF3  1<<8 
#define TCIF3 1<<9 
#define HTIF3 1<<10 
#define TEIF3 1<<11 
#define GIF4  1<<12 
#define TCIF4 1<<13 
#define HTIF4 1<<14 
#define TEIF4 1<<15 
#define GIF5  1<<16 
#define TCIF5 1<<17 
#define HTIF5 1<<18 
#define TEIF5 1<<19 
#define GIF6  1<<20 
#define TCIF6 1<<21 
#define HTIF6 1<<22 
#define TEIF6 1<<23 
#define GIF7  1<<24 
#define TCIF7 1<<25 
#define HTIF7 1<<26 
#define TEIF7 1<<27 

#define MSIZE_B     0
#define MSIZE_HW    1<<10
#define MSIZE_W     2<<10

#define PSIZE_B     0 
#define PSIZE_HW    1<<10
#define PSIZE_W     2<<10

#define MINC_EN    1<<7
#define MINC_OFF   0
#define PINC_EN    1<<6
#define PINC_OFF   0

#define DIR_Per2Mem   0
#define DIR_Mem2Per   1<<4

#define DMA_TEIE 1<<3
#define DMA_HTIE 1<<2
#define DMA_TCIE 1<<1

#define CCR_EN     1<<0
#define CCR_OFF  ~(1<<0)

#define TRANSFER_MASK 0xF01E
#define MAX_TRANSFER  0xFFFF

typedef struct{
    volatile uint32_t ISR;          //0x00
    volatile uint32_t IFCR;         //0x04
} DMA_Status_t;

typedef struct {
    volatile uint32_t CCR;         //0x08
    volatile uint32_t CNDTR;       //0X0C
    volatile uint32_t CPAR;        //0x10
    volatile uint32_t CMAR;        //0x14
    volatile uint32_t Reserved;    //0x18
} DMA_TypeDef;

#define DMA1_Status_Reg ((DMA_Status_t *)  0x40020000)
#define DMA1_CH1        ((DMA_TypeDef *)   0x40020008)
#define DMA1_CH2        ((DMA_TypeDef *)   0x4002001C)
#define DMA1_CH3        ((DMA_TypeDef *)   0x40020030)
#define DMA1_CH4        ((DMA_TypeDef *)   0x40020044)
#define DMA1_CH5        ((DMA_TypeDef *)   0x40020058)
#define DMA1_CH6        ((DMA_TypeDef *)   0x4002006C)
#define DMA1_CH7        ((DMA_TypeDef *)   0x40020080)

#define DMA2_Status_Reg ((DMA2_Status_t *) 0x40020400)
#define DMA2_CH1        ((DMA_TypeDef *)   0x40020408)
#define DMA2_CH2        ((DMA_TypeDef *)   0x4002041C)
#define DMA2_CH3        ((DMA_TypeDef *)   0x40020430)
#define DMA2_CH4        ((DMA_TypeDef *)   0x40020444)
#define DMA2_CH5        ((DMA_TypeDef *)   0x40020458)
 
void DMA1_Init(void);
void configure_dma_spi(SPI_TypeDef* SPIx);

#endif
