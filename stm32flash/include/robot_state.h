#ifndef ROBOT_STATE_H
#define ROBOT_STATE_H

#include <stdint.h>

typedef struct {
    int16_t  x; // position, endyne (divide by 48 = cm)
    int16_t  y;
    int16_t  heading;   // heading, degrees: 0=N, 90=E, 180=S, 270=W
    int16_t prev_tof;   
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
int nav_time_exceeded(uint32_t ms);

int nav_wall_ahead(void);
// drive foward until ToF is close enough
int nav_drive_to_wall(void) ;

// drive exactly this many cm
int nav_step_cm(int8_t dist_cm);

// rotate to a target heading
int nav_rotate_to(int16_t target_hdg);


// drive west until passed x threshold
int nav_drive_past_x(int16_t x_thresh);

// drive toward a specific x,y target
// returns 1 when close enough
int nav_drive_to_xy(int16_t tx, int16_t ty);

uint16_t cam_read(void);

#endif
