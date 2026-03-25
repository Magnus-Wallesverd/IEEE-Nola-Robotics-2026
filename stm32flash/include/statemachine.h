#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdint.h>

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
    MISSION_WP_RECOVER,   // navigate to a specific waypoint when stuck
    MISSION_CAVE_ENTER,   // align and enter cave
    MISSION_LAWN_CAVE,    // lawnmower sweep inside cave
    MISSION_EXIT_CAVE,    // align and exit cave
    MISSION_DROPOFF,      // go to rendezvous pad and release hopper
    MISSION_DONE,
    MISSION_COUNT
} Mission;

typedef struct {
    Mission  mission;
    Mission  prev_mission;

    int      sub_step;          // step within current mission
    int      wp_target_idx;     // which waypoint we're driving to (-1 = none)
    int      wp_nav_step;       // step within wp_nav_to
    int      row_parity;        // 0 = heading north, 1 = heading south
    int      in_cave;           // 1 if we're inside the cave
    int      dropoff_needed;    // 1 when hopper is full enough to drop off

    uint32_t last_progress_tick;
} SM;

void sm_init(SM *s);
void sm_tick(SM *s);
void sm_main(void *args);

uint16_t wp_dist(Waypoint *w);
void     wp_check_visit(SM *s);
int      wp_nearest(SM *s, WpType type);
int      wp_all_done(WpType type);
int      wp_nav_to(SM *s);
 
void ball_detected(void);   // IR break-beam?

#endif
