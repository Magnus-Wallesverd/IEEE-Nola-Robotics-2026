/*
 * simulation main file
 *
 * */

#include "stm32f303.h"
#include <stdio.h>
#include <stdint.h>


void main(void){
    SetPinOutput(&gpioa, 0xFF);
    PinWrite(&gpioa, 0xFF);
    SetPinAlternate(&gpioa, 0xFF00);
    AlternateFunctionSet(&gpioa, 0xFFF0, 2);
}
