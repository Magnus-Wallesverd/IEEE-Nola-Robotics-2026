#include "statemachine.h"
#include "robot_state.h"
#include "motors.h"
#include "usart.h"
#include "lock.h"
#include "tcb.h"
#include <stdint.h>

motor_t motor_sweep_table[] = {
    step3,
    rotate3,
    step3,
    rotate3,
    step3,
    rotate3,
    step3,
    rotate3,
};

motor_payload motor_sweep_payload_table[] = {
    {.args = 60,  .speed = 2},
    {.args = 1440,  .speed = 1},
    {.args = 60,  .speed = 2},
    {.args = 2880,  .speed = 1},
    {.args = 60,  .speed = 2},
    {.args = 4320,  .speed = 1},
    {.args = 60,  .speed = 2},
    {.args = 0,  .speed = 1},

};

// const motor_item_t sweep[] = {
//     {.fn = step3, .args  = (void*) }
// }
//

transport_item_t usart_frame;
usart_payload usart_data = {
    .f_ID = 1, .LSB = 0xB, .MSB =0xB  
};

void motor_scheduler(motor_item_t* motor_task, motor_t fn, motor_payload* payload){
        // motor_task->fn = fn;
        // motor_task->args = (void*)payload;
        // enqueue(motor_queue_ptr, (void*)motor_task);
        mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 8);

}

void usart_test(int x){
    for(int i = 0; i < x; i++){
        usart_frame.fn = usart_load_tx; 
        usart_frame.args = (void*)&usart_data;
        enqueue(transport_queue_ptr, (void*)&usart_frame);
    }
}

int check_handler(queue_t* q){
    if( q->count == 0){
        return 1;
    }
    return 0;
}

void sm_main(void *args) {
    (void)args;
    while(!*ToF_Distance_p);
    // usart_test(4);
    mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 8);
    while(1);
    if(check_handler(motor_queue_ptr)&check_handler(transport_queue_ptr)){
        
    } else {
        yield();
    }

    robot_state_init();
    SM s;
    sm_init(&s);
    while (s.mission != MISSION_DONE) {
        robot_state_update();
        sm_tick(&s);
    }
    turn_off_motors();
    while (1) { block(); }
}



