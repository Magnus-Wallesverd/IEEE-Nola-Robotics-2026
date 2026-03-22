#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdint.h>

typedef struct { int32_t x, y; } Vec2;

typedef struct { 
    uint16_t x, y;
    uint8_t type;
    uint8_t visited; 
} Waypoint;

typedef enum {
    MISSION_LAWN_OPEN,
    MISSION_WP_RECOVER,
    MISSION_CAVE_ENTER,
    MISSION_LAWN_CAVE,
    MISSION_EXIT_CAVE,
    MISSION_DONE,
    MISSION_COUNT
} Mission;

typedef enum { 
    WP_OPEN=0, 
    WP_CAVE_ENTER, 
    WP_CAVE, 
    WP_CAVE_EXIT 
} WpType;

typedef struct {
    Mission  mission;
    Mission  prev_mission;

    int16_t  x, y;          // robot centre
    int16_t  heading;       // 0=N 90=E 180=S 270=W

    uint16_t tof_fwd_mm;

    uint8_t  cam_sees;
    uint8_t  cam_id[2];     // [0]=front [1]=rear, 0xFF=none

    uint8_t  telemetry_pad;
   
    int      wp_target_idx;
    int      wp_nav_step;
    int      sub_step;
    int      row_parity; // 0 = heading north, 1 = south
    int      in_cave;

    uint32_t start_tick;
    uint32_t elapsed_ms;
    uint32_t last_progress_tick;
} Robot;

void    robot_init(Robot *r);
void    robot_tick(Robot *r);
void    sensor_update(Robot *r);
void    robot_main(void *args);

uint8_t cam_req(uint8_t fn, uint8_t a1, uint8_t a2);
int16_t cam_req16(uint8_t fn, uint8_t a1);
void pose_correct(Robot *r, uint8_t side);

uint16_t wp_dist(Robot *r, Waypoint *w);
void    wp_check_visit(Robot *r);
int     wp_nearest(Robot *r, WpType type);
int     wp_all_done(WpType type);
int     wp_nav_to(Robot *r);

int     drive_cm(int8_t dist_cm);
int     turn_to(Robot *r, int16_t target_hdg);

#endif /* STATEMACHNE_H */
