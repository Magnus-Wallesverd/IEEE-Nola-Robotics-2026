#include "statemachine.h"
#include "motors.h"
#include "usart.h"
#include "lock.h"
#include "tcb.h"
#include <stdint.h>

/*
 * FN_SEE_TAG: 0x00
 * FN_ID: 0x01
 * FN_DDIST: 0x02
 * FN_ANG = 0x03
 * FN_YAW = 0x04
 * FN_START = 0x09
 */

// yall im sorry abt this file  

// Endyne Units: raw / 48 = cm
// Heading: 0=N 90=E 180=S 270=W


/* ROBOT INIT AND UPDATE */
Robot rbt;

void robot_init(void){
    rbt.x = 3810;
    rbt.y = 731;
    rbt.heading = 0;  
    rbt.tof_mm = 0; 
    rbt.telemetry_pad = 0; 
    rbt.start_detected = 0;           
    rbt.elapsed_ms = 0;
    rbt.prev_tof = 0;
    rbt.count_geo = 0;
}

void robot_state_update(void *args){
    (void)args;
    while(1){
        rbt.heading = bno_heading / 16; // degrees
        uint16_t tof_now = *ToF_Distance_p;
        int16_t delta_tof = (rbt.prev_tof - tof_now) * 48 / 10; // endyne      
        
        rbt.elapsed_ms = get_global_tick();
        block();
    }
}

/* MISSIONS */

int wait_start(){
    while(!rbt.start_detected){ block(); }
    return MISSION_SWEEP_WEST;
} 

int sweep_west(){
    motor_t motor_sweep_table[] = {
        rotate3,
        rotate3,
        step3,
        rotate3,
        step3,
        rotate3,
    };

    motor_payload motor_sweep_payload_table[] = {
        {.args = 300*16,  .speed = 1},
        {.args = 270*16,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 0,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 90*16,  .speed = 1},
    };

    mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);

    return MISSION_GRAB_NEB;

}

int grab_neb(){
    motor_t motor_sweep_table[] = {
        rotate3,
        rotate3,
        step3,
        rotate3,
        step3,
        rotate3,
    };

    motor_payload motor_sweep_payload_table[] = {
        {.args = 300*16,  .speed = 1},
        {.args = 270*16,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 0,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 90*16,  .speed = 1},
    };

    mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);

    return MISSION_GRAB_NEB;
}

int grab_geo(){
    motor_t motor_sweep_table[] = {
        rotate3,
        rotate3,
        step3,
        rotate3,
        step3,
        rotate3,
    };

    motor_payload motor_sweep_payload_table[] = {
        {.args = 300*16,  .speed = 1},
        {.args = 270*16,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 0,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 90*16,  .speed = 1},
    };

    mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);

    return MISSION_GRAB_NEB;
}

int lawn_open(){
    motor_t motor_sweep_table[] = {
        rotate3,
        rotate3,
        step3,
        rotate3,
        step3,
        rotate3,
    };

    motor_payload motor_sweep_payload_table[] = {
        {.args = 300*16,  .speed = 1},
        {.args = 270*16,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 0,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 90*16,  .speed = 1},
    };

    mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);

    return MISSION_GRAB_NEB;
}

int align_to_cave(){
    motor_t motor_sweep_table[] = {
        rotate3,
        rotate3,
        step3,
        rotate3,
        step3,
        rotate3,
    };

    motor_payload motor_sweep_payload_table[] = {
        {.args = 300*16,  .speed = 1},
        {.args = 270*16,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 0,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 90*16,  .speed = 1},
    };

    mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);

    return MISSION_GRAB_NEB;
}

int lawn_cave(){
    motor_t motor_sweep_table[] = {
        rotate3,
        rotate3,
        step3,
        rotate3,
        step3,
        rotate3,
    };

    motor_payload motor_sweep_payload_table[] = {
        {.args = 300*16,  .speed = 1},
        {.args = 270*16,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 0,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 90*16,  .speed = 1},
    };

    mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);

    return MISSION_GRAB_NEB;
}

int dropoff(){
    motor_t motor_sweep_table[] = {
        rotate3,
        rotate3,
        step3,
        rotate3,
        step3,
        rotate3,
    };

    motor_payload motor_sweep_payload_table[] = {
        {.args = 300*16,  .speed = 1},
        {.args = 270*16,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 0,  .speed = 1},
        {.args = 100,  .speed = 2},
        {.args = 90*16,  .speed = 1},
    };

    mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);

    return MISSION_GRAB_NEB;
}

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
    {.args = 4320,  .speed = 1},
    {.args =-60,  .speed = 2},
    {.args = 0,  .speed = 1},
};

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
    robot_state_init();

    Mission mission = MISSION_WAIT_START;

    while(mission != MISSION_DONE) {
        switch(mission) {
            case MISSION_WAIT_START:
                mission = wait_start();
                break;
            case MISSION_SWEEP_WEST:
                mission = sweep_west();
                break;
            case MISSION_GRAB_NEB:
                mission = grab_neb();
                break;
            case MISSION_GRAB_GEO:
                mission = grab_geo();
                break;
            case MISSION_LAWN_OPEN:
                mission = lawn_open();
                break;
            case ALIGN_TO_CAVE:
                mission = align_to_cave();
                break;
            case MISSION_LAWN_CAVE:
                mission = lawn_cave();
                break;
            case MISSION_DROPOFF:
                mission = dropoff();
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



