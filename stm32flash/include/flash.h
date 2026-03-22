#ifndef FLASH_H
#define FLASH_H

#define FLASH_ACR_LATENCY0 0
#define FLASH_ACR_LATENCT1 1
#define FLASH_ACR_LATENCY2 2
#define FLASH_ACR_LATENCY3 3
#define FLASH_ACR_LATENCY4 4

typedef struct{
    volatile uint32_t ACR;      // 0x00
} Flash_TypeDef;

#define FLASH ((Flash_TypeDef *) 0x40022000)

#endif // !FLASH_H
