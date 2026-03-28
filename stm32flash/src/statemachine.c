#include "statemachine.h"
#include "motors.h"
#include "usart.h"
#include "lock.h"
#include "tcb.h"
#include <stdint.h>


// yall im sorry abt this file  

// Endyne Units: raw / 48 = cm
// Heading: 0=N 90=E 180=S 270=W


/* ROBOT INIT AND UPDATE */
//Robot rbt;
//
//void robot_init(void){
//    rbt.x = 3810;
//    rbt.y = 731;
//    rbt.heading = 0;  
//    rbt.tof_mm = 0; 
//    rbt.telemetry_pad = 0; 
//    rbt.start_detected = 0;           
//    rbt.elapsed_ms = 0;
//    rbt.prev_tof = 0;
//    rbt.count_geo = 0;
//}
//
//void robot_state_update(void *args){
//    (void)args;
//    while(1){
//        rbt.heading = bno_heading / 16; // degrees
//        uint16_t tof_now = *ToF_Distance_p;
//        int16_t delta_tof = (rbt.prev_tof - tof_now) * 48 / 10; // endyne      
//        
//        rbt.elapsed_ms = get_global_tick();
//        block();
//    }
//}
//
/* MISSIONS */

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
    {.args = -60,  .speed = 2},
    {.args = 1440,  .speed = 1},
    {.args = -60,  .speed = 2},
    {.args = 2880,  .speed = 1},
    {.args = -60,  .speed = 2},
    {.args = 1440,  .speed = 1},
    {.args =-60,  .speed = 2},
    {.args = 0,  .speed = 1},
};

motor_payload rotate_left = { .args = 270, .speed = 1};
motor_payload step60 = { .args = -60, .speed = 2};

int wait_start(){
    while(!start_detected){ block(); }
    return LAWN_OPEN;
} 

int lawn_open(){
//    motor_task->fn = rotate3;
//    motor_task->args = &rotate_left;
//    enqueue(motor_queue_ptr, (void*)motor_task);
//
//    motor_task->fn = step60;
//    motor_task->args = &step60;
//    enqueue(motor_queue_ptr, (void*)motor_task);
//
//    mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);
//    mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);
//    mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);
//
    return ALIGN_CAVE;
}

int align_to_cave(){


    return LAWN_CAVE;
}

int lawn_cave(){


    mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);

    return MISSION_DONE;
}

// const motor_item_t sweep[] = {
//     {.fn = step3, .args  = (void*) }
// }

//void usart_test(int x){
//    for(int i = 0; i < x; i++){
//        usart_frame.fn = usart_load_tx; 
//        usart_frame.args = (void*)&usart_data;
//        enqueue(transport_queue_ptr, (void*)&usart_frame);
//    }
//}

//transport_item_t usart_frame;
// usart_payload usart_data = {
//    .f_ID = 1, .LSB = 0xB, .MSB =0xB  
//};

void motor_scheduler(motor_item_t* motor_task, motor_t fn, motor_payload* payload){
        // motor_task->fn = fn;
        // motor_task->args = (void*)payload;
        // enqueue(motor_queue_ptr, (void*)motor_task);
        mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 8);

}

int check_handler(queue_t* q){
    if( q->count == 0){
        return 1;
    }
    return 0;
}

void sm_main(void *args) {
    (void)args;
    // robot_state_init();
    

    Mission mission = WAIT_START;

    while(1);
    while(mission != MISSION_DONE) {
        switch(mission) {
            case WAIT_START:
                mission = wait_start();
                break;
            case LAWN_OPEN:
                mission = lawn_open();
                break;
            case ALIGN_CAVE:
                mission = align_to_cave();
                break;
            case LAWN_CAVE:
                mission = lawn_cave();
                break;
            default:
                mission = MISSION_DONE;
                break;
        }
    }
    
    if(check_handler(motor_queue_ptr)&check_handler(transport_queue_ptr)){
        
    } else {
        yield();
    }

    turn_off_motors();
    while (1) { block(); }
}



