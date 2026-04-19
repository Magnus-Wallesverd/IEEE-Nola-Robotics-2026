#ifndef FLASH_H
#define FLASH_H

#define FLASH_ACR_LATENCY0 0
#define FLASH_ACR_LATENCT1 1
#define FLASH_ACR_LATENCY2 2

typedef struct{
    volatile uint32_t ACR;      // 0x00
    volatile uint32_t KEYR;     // 0x04
    volatile uint32_t OPTKEYR;  // 0x08
    volatile uint32_t SR;       // 0x0C
    volatile uint32_t CR;       // 0x10
    volatile uint32_t AR;       // 0x14
    volatile uint32_t RESERVED; // 0x18
    volatile uint32_t OBR;      // 0x1C
    volatile uint32_t WRPR;     // 0x20
} Flash_TypeDef;

#define FLASH ((Flash_TypeDef *) 0x40022000)

#endif // !FLASH_H
