#ifndef ROBOT_STATE_H
#define ROBOT_STATE_H

#include <stdint.h>

typedef struct {
    int16_t  x; // position, endyne (divide by 48 = cm)
    int16_t  y;
    int16_t  heading;   // heading, degrees: 0=N, 90=E, 180=S, 270=W

    uint16_t tof_mm;    // tof forward distance, mm
    uint16_t  cam_sees;  // which cam sees a tag: bit0=front, bit1=rear
    uint8_t  cam_id[2]; // which ID: [0]=front [1]=rear, 0xFF if none
    
    uint8_t  telemetry_pad;     // rendezvous pad number (ID 0-4)
    uint8_t  start_detected;    // set to 1 when startLED detected by rear cam_sees

    uint8_t  count_geo;     // magnetic (geodinium)
    uint8_t  count_neb;     // non-magnetic (nebulite)
                            
    uint32_t start_tick;    // match timing
    uint32_t elapsed_ms;

} RobotState;

extern RobotState g_robot;

void robot_state_init(void);
void robot_state_update(void);

#endif
