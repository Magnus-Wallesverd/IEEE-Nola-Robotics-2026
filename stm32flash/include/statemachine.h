#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdint.h>

// robot state
typedef struct {
    int16_t  x; // position, endyne (divide by 48 = cm)
    int16_t  y;
    int16_t  heading;   // heading, degrees: 0=N, 90=E, 180=S, 270=W   
    uint16_t tof_mm;    // tof forward distance, mm
    uint8_t  telemetry_pad;     // rendezvous pad number (ID 0-4)
    uint8_t  start_detected;    // set to 1 when startLED detected by rear cam_sees             
    uint32_t elapsed_ms;
    uint16_t prev_tof;
    int8_t count_geo;
} Robot;

// Waypoint types
typedef enum {
    WP_OPEN = 0,
    WP_CAVE_ENTER,
    WP_CAVE,
    WP_CAVE_EXIT
} WpType;

typedef struct {
    uint16_t x, y;
    uint8_t  type;
    uint8_t  visited;
} Waypoint;

typedef enum {
    MISSION_WAIT_START,   // wait for start LED
    MISSION_SWEEP_WEST,   // read rendezvous and clear dropoff area
    MISSION_GRAB_NEB,     // pick up nebulite container
    MISSION_GRAB_GEO,     // pick up geodinium container
    MISSION_LAWN_OPEN,    // lawnmower sweep open arena
    MISSION_CAVE_ENTER,   // align and enter cave
    MISSION_LAWN_CAVE,    // lawnmower sweep inside cave
    MISSION_DROPOFF,      // go to rendezvous pad and release hopper
    MISSION_DONE,
} Mission;

typedef struct {
    Mission  mission;
    Mission  prev_mission;
    int      dropoff_needed;    // 1 when hopper is full enough to drop off
    uint32_t last_progress_tick;
} SM;

void sm_init(SM *s);
void sm_tick(SM *s);
void sm_main(void *args);

void robot_state_update(void *args);

uint16_t wp_dist(Waypoint *w);
void     wp_check_visit(SM *s);
int      wp_nearest(SM *s, WpType type);
int      wp_all_done(WpType type);
int      wp_nav_to(SM *s);
 
void ball_detected(void);   // ??

#endif
