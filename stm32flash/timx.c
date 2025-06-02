/* Hardware Abstraction for Timers */

/*
 * TODO Timer 2,3,4 have some special CR2 bits...
 * TODO Timer sync function
 *
 */

#include "timx.h"


// enable timer 1, 8, or 20 with some loaded preset
// do not use on other timers. unknown behavior
void InitAdvTIM(Adv_TIM_TypeDef *port){
    uint32_t advinitmask = 0b1000001;       // preset
    port->CR1 |= advinitmask;
}

// enable timer 2, 3, or 4
// one timer per call
void InitGenTIM(Gen_TIM_TypeDef * port){
    uint32_t geninitmask = 0b1000001;       //preset
    port->CR1 |= geninitmask;
}
