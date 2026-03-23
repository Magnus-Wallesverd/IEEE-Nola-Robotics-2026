#include "statemachine.h"
#include "robot_state.h"   
#include "nav.h"          
#include "motors.h"        
#include "lock.h"
#include "tcb.h"
#include <stdint.h>

/* ══════════════════════════════════════════════════════════════
   UNITS:  raw / 48 = cm
   Heading: 0=North  90=East  180=South  270=West
   ══════════════════════════════════════════════════════════════ */

// Field geometry
#define X_WEST          (15  * 48)
#define X_EAST          (221 * 48)
#define Y_SOUTH         (15  * 48)
#define Y_NORTH         (99  * 48)
#define X_CAVE_THRESH   (148 * 48)
#define X_CAVE_IN       (165 * 48)
#define Y_CAVE_CENTER   (57  * 48)

#define ROW_STEP_CM     25 // lawnmower row advance

/* Starting position */
#define START_X         3810
#define START_Y         731

/* Navigation thresholds */
#define DIST_THRESH     (5  * 48)   // close enough to waypoint
#define VISITED_THRESH  (15 * 48)   // mark waypoint visited
#define RECOVERY_MS     4000        // stuck timeout
#define DROPOFF_MS      140000      // 2:20

/* WAYPOINTS */
Waypoint wps[] = {
    // Open arena
    {  15*48, 15*48, WP_OPEN, 0 },
    { 140*48, 15*48, WP_OPEN, 0 },
    { 140*48, 40*48, WP_OPEN, 0 },
    {  15*48, 40*48, WP_OPEN, 0 },
    {  15*48, 65*48, WP_OPEN, 0 },
    { 140*48, 65*48, WP_OPEN, 0 },
    { 140*48, 90*48, WP_OPEN, 0 },
    {  15*48, 90*48, WP_OPEN, 0 },
    // Cave enter
    { 155*48, 57*48, WP_CAVE_ENTER, 0 },
    { 172*48, 57*48, WP_CAVE_ENTER, 0 },
    // cave
    { 172*48, 15*48, WP_CAVE, 0 },
    { 221*48, 15*48, WP_CAVE, 0 },
    { 221*48, 40*48, WP_CAVE, 0 },
    { 172*48, 40*48, WP_CAVE, 0 },
    { 172*48, 65*48, WP_CAVE, 0 },
    { 221*48, 65*48, WP_CAVE, 0 },
    { 221*48, 90*48, WP_CAVE, 0 },
    { 172*48, 90*48, WP_CAVE, 0 },
    // exit cave
    { 155*48, 57*48, WP_CAVE_EXIT, 0 },
};

#define NUM_WP (sizeof(wps) / sizeof(wps[0]))

/* Waypoint Helpers */
uint16_t wp_dist(Waypoint *w) {
    int16_t dx = (g_robot.x - (int16_t)w->x) / 48;
    int16_t dy = (g_robot.y - (int16_t)w->y) / 48;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return (uint16_t)(dx + dy) * 48;
}

void wp_check_visit(SM *s) {
    (void)s;
    for (int i = 0; i < (int)NUM_WP; i++) {
        if (!wps[i].visited && wp_dist(&wps[i]) <= VISITED_THRESH)
            wps[i].visited = 1;
    }
}

int wp_nearest(SM *s, WpType t) {
    (void)s;
    int      nearest  = -1;
    uint16_t shortest = 0xFFFF;
    for (int i = 0; i < (int)NUM_WP; i++) {
        if (!wps[i].visited && (WpType)wps[i].type == t) {
            uint16_t d = wp_dist(&wps[i]);
            if (d < shortest) { shortest = d; nearest = i; }
        }
    }
    return nearest;
}

int wp_all_done(WpType t) {
    for (int i = 0; i < (int)NUM_WP; i++) {
        if (!wps[i].visited && (WpType)wps[i].type == t) return 0;
    }
    return 1;
}

// nav to waypoint target, return 1 when reached
int wp_nav_to(SM *s) {
    if (s->wp_target_idx < 0) return 1;
    Waypoint *wp = &wps[s->wp_target_idx];

    switch (s->wp_nav_step) {

    case 0:
        if (!nav_rotate_to(g_robot.x < (int16_t)wp->x ? 90 : 270))
            return 0;
        s->wp_nav_step = 1;
        return 0;

    case 1: {
        int16_t dx = (int16_t)wp->x - g_robot.x;
        if (dx < 0) dx = -dx;
        if (dx < DIST_THRESH) { s->wp_nav_step = 2; return 0; }
        int8_t cm = (int8_t)((dx/48 > 120) ? 120 : dx/48);
        nav_step_cm(cm);
        return 0;
    }

    case 2:
        if (!nav_rotate_to(g_robot.y < (int16_t)wp->y ? 0 : 180))
            return 0;
        s->wp_nav_step = 3;
        return 0;

    case 3: {
        int16_t dy = (int16_t)wp->y - g_robot.y;
        if (dy < 0) dy = -dy;
        if (dy < DIST_THRESH) {
            wps[s->wp_target_idx].visited = 1;
            s->last_progress_tick = get_global_tick();
            s->wp_nav_step   = 0;
            s->wp_target_idx = -1;
            return 1;
        }
        int8_t cm = (int8_t)((dy/48 > 120) ? 120 : dy/48);
        nav_step_cm(cm);
        return 0;
    }

    }
    return 1;
}

/* Mission Handlers */

// sub_step 0: drive forward until wall
// sub_step 1: turn east
// sub_step 2: step one row width east
// sub_step 3: flip direction, back to sub_step 0
Mission handle_lawn_open(SM *s) {
    if (nav_time_exceeded(DROPOFF_MS)) return MISSION_EXIT_CAVE;

    if (get_global_tick() - s->last_progress_tick > RECOVERY_MS) {
        int i = wp_nearest(s, WP_OPEN);
        if (i >= 0) {
            s->wp_target_idx    = i;
            s->wp_nav_step      = 0;
            s->prev_mission     = MISSION_LAWN_OPEN;
            s->sub_step         = 0;
            return MISSION_WP_RECOVER;
        }
    }

    if (wp_all_done(WP_OPEN)) { s->sub_step = 0; return MISSION_CAVE_ENTER; }

    wp_check_visit(s);

    switch (s->sub_step) {

    case 0:
        if (nav_drive_to_wall()) {
            g_robot.y = (s->row_parity == 0) ? Y_NORTH : Y_SOUTH;
            s->last_progress_tick = get_global_tick();
            s->sub_step = 1;
        }
        return MISSION_LAWN_OPEN;

    case 1:
        if (nav_rotate_to(90)) s->sub_step = 2;
        return MISSION_LAWN_OPEN;

    return MISSION_DONE;
}

Mission handle_done(Robot *r){
    (void)r;
    turn_off_motors();
    return MISSION_DONE;
}

/* DISPATCH TABLE */
typedef Mission (*MissionHandler)(Robot *);

MissionHandler mission_handlers[MISSION_COUNT] = {
    [MISSION_LAWN_OPEN]= handle_lawn_open,
    [MISSION_WP_RECOVER]= handle_wp_recover,
    [MISSION_CAVE_ENTER]= handle_cave_enter,
    [MISSION_LAWN_CAVE]  = handle_lawn_cave,
    [MISSION_EXIT_CAVE] = handle_exit_cave,
    [MISSION_DONE]      = handle_done,
};

/*
MissionHandler mission_handlers[MISSION_COUNT] = {
    [MISSION_SWEEP_WEST]= handle_sweep_west,
    [MISSION_FACE_NEB]  = handle_face_neb,
    [MISSION_GRAB_NEB]  = handle_grab_neb,
    [MISSION_DROP_NEB]  = handle_drop_neb,
    [MISSION_FACE_GEO]  = handle_face_geo,
    [MISSION_GRAB_GEO]  = handle_grab_geo,
    [MISSION_DROP_GEO]  = handle_drop_geo,
    [MISSION_LAWN_START]= handle_lawn_start,
    [MISSION_LAWN_ROW]  = handle_lawn_row,
    [MISSION_WP_RECOVER]= handle_wp_recover,
    [MISSION_CAVE_ENTER]= handle_cave_enter,
    [MISSION_CAVE_START]= handle_cave_start,
    [MISSION_CAVE_ROW]  = handle_cave_row,
    [MISSION_EXIT_CAVE] = handle_exit_cave,
    [MISSION_DROP_BAGS] = handle_drop_bags,
    [MISSION_DONE]      = handle_done,
};
*/

/* ══════════════════════════════════════════════════════════════
   TICK + ENTRY POINT
   ══════════════════════════════════════════════════════════════ */
void robot_tick(Robot *r){
    sensor_update(r);
    Mission next = mission_handlers[r->mission](r);
    if(next != r->mission){
        r->prev_mission = r->mission; 
        r->mission = next;
        r->sub_step = 0;
    }
}

void robot_main(void *args){
    (void)args; 
    Robot r; 
    robot_init(&r);
    while(r.mission != MISSION_DONE) 
        robot_tick(&r);
    turn_off_motors(); 
    while(1){ block();}
}
