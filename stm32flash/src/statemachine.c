#include "statemachine.h"
#include "motors.h"
#include "usart.h"
#include "lock.h"
#include "tcb.h"
#include <stdint.h>
uint16_t N_tof =0;
uint16_t S_tof =0;
uint16_t E_tof =0;
uint16_t W_tof =0;
uint16_t gx =0;
uint16_t gy = 0;

 
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
int robot_pos_update(void* args){
    int16_t direction = ((motor_payload*)args)->args;
    switch(direction){
    case 0:
        N_tof = (*ToF_Distance_p);
        break;
    case 1:
        S_tof = (*ToF_Distance_p);
        break;
    case 2:
        E_tof = (*ToF_Distance_p);
        break;
    case 3:
        W_tof = (*ToF_Distance_p);
        break;
    case 4:
        if((S_tof + N_tof)/10 >=  87)
            gy = S_tof/10;
        if((E_tof + W_tof)/10 >=  199)
            gx = E_tof/10;
        break;
    }
    return 0;
}
/* MISSIONS */
motor_t cave_enter[] = {
    step3,
    rotate3,
    step3,
};
motor_payload cave_enter_args[] = {
    {.args = 44,  .speed = 2},
    {.args = 1440,  .speed = 1},
    {.args = 100,  .speed = 1},
};

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


motor_payload align_payload[] = {
    {.args = 0,  .speed = 1},
    {.args = 0,  .speed = 1}
};

motor_t get_global_pos[] = {
    rotate3,
    robot_pos_update,
    rotate3,
    robot_pos_update,
    rotate3,
    robot_pos_update,
    rotate3,
    robot_pos_update,
    robot_pos_update
};

motor_payload global_pos_args[] = {
    {.args = 0,  .speed = 1},
    {.args = 0,  .speed = 1}, // check norh
    {.args = 1440,  .speed = 1},
    {.args = 1,  .speed = 1}, // check south
    {.args = 2880,  .speed = 1},
    {.args = 2,  .speed = 1}, //
    {.args = 4320,  .speed = 1},
    {.args = 3,  .speed = 1}, // 
    {.args = 4,  .speed = 1}, // 
};

motor_t align[] = {
    rotate3,
    step3,
};

int alignY(void* args){
    mass_enqueue(get_global_pos, global_pos_args, 9);
    align_payload[1].args = 67 - gy;
    mass_enqueue(align, align_payload, 2);
    return 1;

}


motor_payload rotate_left = { .args = 270, .speed = 1};
motor_payload step60 = { .args = -60, .speed = 2};

int wait_start(){
    while(!start_detected){ block(); }
    return LAWN_OPEN;
} 

// int lawn_open(){
//     motor_task->fn = rotate3;
//     motor_task->args = &rotate_left;
//     enqueue(motor_queue_ptr, (void*)motor_task);
//
//     motor_task->fn = step60;
//     motor_task->args = &step60;
//     enqueue(motor_queue_ptr, (void*)motor_task);
//
//     mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);
//     mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);
//     mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);
//
//     return ALIGN_CAVE;
// }


// int lawn_cave(){
//
//
//     mass_enqueue(motor_sweep_table, motor_sweep_payload_table, 6);
//
//     return MISSION_GRAB_NEB;
// }

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
    int steps =0;

    Mission mission = WAIT_START;
    
    //while(mission != MISSION_DONE) {
    //     switch(mission) {
    //         case WAIT_START:
    //             mission = wait_start();
    //             break;
    //         case LAWN_OPEN:
    //             mission = lawn_open();
    //             break;
    //         case ALIGN_CAVE:
    //             mission = align_to_cave();
    //             break;
    //         case LAWN_CAVE:
    //             mission = lawn_cave();
    //             break;
    //         default:
    //             mission = MISSION_DONE;
    //             break;
    //     }
    // }
    

    turn_off_motors();
    mass_enqueue(cave_enter, cave_enter_args, 3);

    while (1) { 
        if(motor_tcb->flags ==1){
            steps++;
            if(steps ==1){
                mass_enqueue(motor_sweep_table, motor_sweep_payload_table,8);
            }
            if(steps ==2){
                alignY(0);
            }
            if(steps ==3){
                cave_enter_args[0].args = 0;
                cave_enter_args[1].args = 4320;
                mass_enqueue(cave_enter, cave_enter_args, 3);

            }
            
        } else {
            yield();
        }

    }
}
