#ifndef ROBOT_STATE_H
#define ROBOT_STATE_H

#include <stdint.h>

typedef struct {

    /* Position and heading */
    int16_t  x;             // raw units, /48 = cm, origin SW corner
    int16_t  y;
    int16_t  heading;       // degrees: 0=N  90=E  180=S  270=W

    /* Sensors */
    uint16_t tof_fwd_mm;    // ToF forward distance in mm
    uint8_t  cam_sees;      // bit0=front visible, bit1=rear
    uint8_t  cam_id[2];     // [0]=front [1]=rear tag ID, 0xFF=none 
    uint8_t  telemetry_pad; // rendezvous pad
    uint8_t  start_detected;// checks if start LED on

    /* Match timing */
    uint32_t start_tick;
    uint32_t elapsed_ms;

} RobotState;

// global instance
extern RobotState g_robot;

void robot_state_init(void);
void robot_state_update(void);

#endif /* ROBOT_STATE_H */
