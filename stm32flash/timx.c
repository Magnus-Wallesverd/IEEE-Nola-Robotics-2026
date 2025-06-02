/* Hardware Abstraction for Timers */

/*
 * TODO Timer 2,3,4 have some special CR2 bits...
 * TODO Timer sync function
 *
 */

#include "timx.h"

const unsigned char initmask = 0b10000001;  //preset

// enable timer 1, 8, or 20 with some loaded preset
// do not use on other timers. unknown behavior
void InitAdvTIM(Adv_TIM_TypeDef *port){
    port->CR1 |= initmask;
}

// enable timer 2, 3, or 4
// one timer per call
void InitGenTIM(Gen_TIM_TypeDef *port){
    port->CR1 |= initmask;
}

// enable timer 6,7
void InitBasicTIM(Basic_TIM_TypeDef *port){
    port->CR1 |= initmask;
}
