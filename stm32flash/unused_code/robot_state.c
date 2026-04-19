#include "robot_state.h"
#include "i2c.h"
#include "usart.h"
#include "motors.h"     
#include "tcb.h"        
#include "semaphore.h"
#include "lock.h"
#include <stdint.h>

// delta_tof (encoder position delta)



// usart_payload ??? = {
//     .f_ID = FN_ID, .LSB = 1, .MSB =0x0  
// };

RobotState g_robot;


int complete_flag = 0;

// sin/cos lookup for the 4 cardinal directions: 0=N 1=E 2=S 3=W
const int8_t cx[4] = {0, 1, 0, -1};
const int8_t cy[4] = {1, 0, -1, 0};

// Read one byte back from the camera rx buffer
uint16_t cam_read(void) {
    uint16_t calculated =0;
    uint8_t *rx = get_usart_rx();
    if (rx[0] == 0xAA && rx[3] == 0){
        calculated = (rx[2] << 8) | rx[1];
        return calculated;
    }
    return 0xFFFF;
}

// void robot_state_init(void) {
//     g_robot.x = 3810;   // starting x
//     g_robot.y = 731;    // starting y
//     g_robot.heading = 0;  // facing north
//     g_robot.tof_mm = 9999;
//     g_robot.prev_tof = 9999;
//     g_robot.cam_sees = 0;
//     g_robot.cam_id[0] = 0xFF;
//     g_robot.cam_id[1] = 0xFF;
//     g_robot.telemetry_pad = 2;  // default to pad 2
//     g_robot.start_detected = 0;
//     g_robot.count_geo = 0;
//     g_robot.count_neb = 0;
//     g_robot.start_tick = get_global_tick();
//     g_robot.elapsed_ms = 0;
// }
/*
 * 0 = waiting on start LED
 * 1 = Reading start led response
 * 2 - n-1 = stuff after
 */

void robot_state_update(void){

}

// void robot_state_update(void) {
//     g_robot.heading  = bno_heading / 16; // divide by 16 for degrees
//     g_robot.tof_mm   = *ToF_Distance_p;
//     if (g_robot.prev_tof == 9999) g_robot.prev_tof = *ToF_Distance_p;
//
//     // match time
//     g_robot.elapsed_ms = get_global_tick() - g_robot.start_tick;
//
//     // projected x/y using current heading quadrant
//     int16_t delta_cm = (g_robot.prev_tof - g_robot.tof_mm) / 10; // cm travelled
//     if (delta_cm != 0) {
//         int idx = ((g_robot.heading + 45) / 90) % 4;
//         g_robot.x += (int16_t)(delta_cm * 48 * cx[idx]);
//         g_robot.y += (int16_t)(delta_cm * 48 * cy[idx]);
//         delta_cm = 0;
//     }
//
//     g_robot.prev_tof = g_robot.tof_mm;
//
//     // what the cam sees
//     usart_load_tx(FN_SEE_TAG, 0, 0);
//     if(wait(&usart_sem)){
//         g_robot.cam_sees = cam_read();
//     }else {
//         yield();
//     }
//
//
//     // if front cam sees a tag, read ID
//     if (g_robot.cam_sees & 0x01) {
//         usart_load_tx(FN_ID, 1, 0);
//         if(wait(&usart_sem)){
//             g_robot.cam_sees = cam_read();
//         }
//
//         // if it's 0-4 -> telemetry pad
//         if (g_robot.cam_id[0] <= 4)
//             g_robot.telemetry_pad = g_robot.cam_id[0];
//     } else {
//         g_robot.cam_id[0] = 0xFF;
//     }
//
//     // if rear cam sees a tag, read ID
//     if (g_robot.cam_sees & 0x02) {
//         usart_load_tx(FN_ID, 2, 0);
//         if(wait(&usart_sem)){
//             g_robot.cam_id[1] = cam_read();
//         }
//
//         // if it's 0-4 -> telemetry pad
//         if (g_robot.cam_id[1] <= 4)
//             g_robot.telemetry_pad = g_robot.cam_id[1];
//     } else {
//         g_robot.cam_id[1] = 0xFF;
//     }
//
//     // check for start LED 
//     if (!g_robot.start_detected) {
//         usart_load_tx(FN_START, 0, 0);
//         if(wait(&usart_sem)){
//             if (cam_read() == 1) g_robot.start_detected = 1;
//
//         }
//     }

//    {   // this is for sorting
//        int32_t mag_sq = (int32_t)mag_data[0] * mag_data[0]
//                       + (int32_t)mag_data[1] * mag_data[1]
//                       + (int32_t)mag_data[2] * mag_data[2];
//        (void)mag_sq;
//    }
// }
//
// sorting 
// void ball_detected(void) {
//    mag_read();  // refresh mag_data from the sensor right now
//
//    int32_t mag_sq = (int32_t)mag_data[0] * mag_data[0]
//                   + (int32_t)mag_data[1] * mag_data[1]
//                   + (int32_t)mag_data[2] * mag_data[2];
//
//    if (mag_sq > MAG_THRESHOLD_SQ)
//        g_robot.count_geo++;
//    else
//        g_robot.count_neb++;
// }
