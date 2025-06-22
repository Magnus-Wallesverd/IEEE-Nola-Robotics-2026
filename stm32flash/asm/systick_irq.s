.text
.global systick_irq
.type systick_irq, %function

systick_irq:
    movw r4,  #0xFA
    movw r5,  #0xFB
    movw r6,  #0xFC
    movw r7,  #0xFD
    push {r4-r7}

set: 
    movw r4, #0xFF

loop:
    sub r4, #0x1
    cmp r4, #0x0
    beq set
    b loop
