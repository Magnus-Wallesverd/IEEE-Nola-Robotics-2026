.text
.global load_sp
.type load_sp, %function

load_sp:
  
  bx lr

.global save_sp
.type save_sp, %function

/*
 * the caller sends the address that the stack pointer needs to go into
 * 
 */

save_sp:
  mov r1, sp
  str r1, [r0]
  bx lr

.global move_save_sp
.type move_save_sp, %function
