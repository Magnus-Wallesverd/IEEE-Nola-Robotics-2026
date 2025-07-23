#ifndef MOTOR_H
#define MOTOR_H
extern uint32_t d1;
extern uint32_t speed;
void update_speed(void);
void motorcontrol(void* args);

#endif
