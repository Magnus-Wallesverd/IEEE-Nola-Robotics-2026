#include <stdint.h>
#include "backend.h"

// void UsageFault_Handler(void){
//     uint32_t cfsr = SCB->CFSR;
//     while(1);
// }
void HardFault_Handler(void)
{
    __asm volatile
    (
        "TST lr, #4            \n"
        "ITE EQ                \n"
        "MRSEQ r0, MSP         \n"
        "MRSNE r0, PSP         \n"
        "B hardfault_c         \n"
    );
}

void hardfault_c(uint32_t *stack)
{
    volatile uint32_t stacked_r0  = stack[0];
    volatile uint32_t stacked_r1  = stack[1];
    volatile uint32_t stacked_r2  = stack[2];
    volatile uint32_t stacked_r3  = stack[3];
    volatile uint32_t stacked_r12 = stack[4];
    volatile uint32_t stacked_lr  = stack[5];
    volatile uint32_t stacked_pc  = stack[6];
    volatile uint32_t stacked_psr = stack[7];

    volatile uint32_t fault_addr = SCB->BFAR;
    volatile uint32_t cfsr = SCB->CFSR;

    while(1);
}

