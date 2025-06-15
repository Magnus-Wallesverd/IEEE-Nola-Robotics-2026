.text
.global systick_irq
.type systick_irq, %function


systick_irq:
    movw r4,  #0xFA
    movw r5,  #0xFB
    movw r6,  #0xFC
    movw r7,  #0xFD
