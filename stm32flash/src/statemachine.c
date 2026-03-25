#include "statemachine.h"
#include "robot_state.h"   
#include "motors.h"        
#include "lock.h"
#include "tcb.h"
#include <stdint.h>

/* ══════════════════════════════════════════════════════════════
   UNITS:  raw / 48 = cm
   Heading: 0=North  90=East  180=South  270=West
   ══════════════════════════════════════════════════════════════ */

/* ── Field geometry (raw units, /48 = cm) ─────────────────────── */
#define X_WEST          (15  * 48)
#define X_EAST          (221 * 48)
#define Y_SOUTH         (15  * 48)
#define Y_NORTH         (99  * 48)
#define X_CAVE_THRESH   (148 * 48)
#define X_CAVE_IN       (165 * 48)
#define Y_CAVE_CENTER   (57  * 48)

#define ROW_STEP_CM     25         

/* ── Starting pose ─────────────────────────────────────────────── */
#define START_X         3810
#define START_Y         731

/* ── Thresholds ─────────────────────────────────────────────────── */
#define HDG_THRESH      5           // degrees, close enough to heading  
#define DIST_THRESH     (5  * 48)   // close enough to waypoint     
#define VISITED_THRESH  (15 * 48)   // mark waypoint as visited     
#define RECOVERY_MS     4000        // ms without progress -> WP recover   
#define DROPOFF_MS      140000      // 2:20 match time limit              
#define TOF_WALL_STOP   200         // mm, wall contact distance         

/* ══════════════════════════════════════════════════════════════
   WAYPOINTS
   ══════════════════════════════════════════════════════════════ */
static Waypoint wps[] = {
    /* Open arena row endpoints */
    {  15*48,  15*48, WP_OPEN,       0 },
    { 140*48,  15*48, WP_OPEN,       0 },
    { 140*48,  40*48, WP_OPEN,       0 },
    {  15*48,  40*48, WP_OPEN,       0 },
    {  15*48,  65*48, WP_OPEN,       0 },
    { 140*48,  65*48, WP_OPEN,       0 },
    { 140*48,  90*48, WP_OPEN,       0 },
    {  15*48,  90*48, WP_OPEN,       0 },
    /* Cave transit */
    { 155*48,  57*48, WP_CAVE_ENTER, 0 },
    { 172*48,  57*48, WP_CAVE_ENTER, 0 },
    /* Cave row endpoints */
    { 172*48,  15*48, WP_CAVE,       0 },
    { 221*48,  15*48, WP_CAVE,       0 },
    { 221*48,  40*48, WP_CAVE,       0 },
    { 172*48,  40*48, WP_CAVE,       0 },
    { 172*48,  65*48, WP_CAVE,       0 },
    { 221*48,  65*48, WP_CAVE,       0 },
    { 221*48,  90*48, WP_CAVE,       0 },
    { 172*48,  90*48, WP_CAVE,       0 },
    /* Cave exit */
    { 155*48,  57*48, WP_CAVE_EXIT,  0 },
};

#define NUM_WP (sizeof(wps) / sizeof(wps[0]))

/* ══════════════════════════════════════════════════════════════
   WAYPOINT HELPERS
   ══════════════════════════════════════════════════════════════ */

/* Manhattan distance from robot to waypoint (raw units) */
uint16_t wp_dist(Waypoint *w) {
    int16_t dx = (g_robot.x - (int16_t)w->x) / 48;
    int16_t dy = (g_robot.y - (int16_t)w->y) / 48;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return (uint16_t)(dx + dy) * 48;
}

/* Mark any waypoint within VISITED_THRESH as visited */
void wp_check_visit(SM *s) {
    (void)s;
    for (int i = 0; i < (int)NUM_WP; i++) {
        if (!wps[i].visited && wp_dist(&wps[i]) <= VISITED_THRESH)
            wps[i].visited = 1;
    }
}

/* Return index of nearest unvisited waypoint of type t, -1 if none */
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

/* Return 1 if all waypoints of type t are visited */
int wp_all_done(WpType t) {
    for (int i = 0; i < (int)NUM_WP; i++) {
        if (!wps[i].visited && (WpType)wps[i].type == t) return 0;
    }
    return 1;
}


int wp_nearest_with_cam_hint(SM *s, WpType t) {
    uint8_t id = g_robot.cam_id[0];    /* front camera tag ID */
    if (id == 0xFF) id = g_robot.cam_id[1];  /* try rear if front empty */

    /* No tag visible — use normal nearest */
    if (id == 0xFF) return wp_nearest(s, t);

    /* Build a bias point from the tag location */
    int16_t bias_x, bias_y;
    if      (id <= 4) { bias_x = X_WEST;  bias_y = Y_CAVE_CENTER; }
    else if (id == 5) { bias_x = START_X; bias_y = Y_NORTH;       }
    else if (id == 6) { bias_x = START_X; bias_y = Y_SOUTH;       }
    else              { bias_x = X_EAST;  bias_y = Y_CAVE_CENTER;  }

    /* Find unvisited waypoint of type t closest to the bias point */
    int      nearest  = -1;
    uint32_t shortest = 0xFFFFFFFF;
    for (int i = 0; i < (int)NUM_WP; i++) {
        if (!wps[i].visited && (WpType)wps[i].type == t) {
            int32_t dx = (int32_t)wps[i].x - bias_x;
            int32_t dy = (int32_t)wps[i].y - bias_y;
            if (dx < 0) dx = -dx;
            if (dy < 0) dy = -dy;
            uint32_t d = (uint32_t)(dx + dy);
            if (d < shortest) { shortest = d; nearest = i; }
        }
    }
    return (nearest >= 0) ? nearest : wp_nearest(s, t);
}

/* ══════════════════════════════════════════════════════════════
   NAVIGATION HELPERS
   ══════════════════════════════════════════════════════════════ */

/* Returns 1 if match time has exceeded ms */
int nav_time_exceeded(uint32_t ms) {
    return g_robot.elapsed_ms >= ms;
}

// Returns 1 if ToF reads wall within stopping distance.
int nav_wall_ahead(void) {
    return g_robot.tof_fwd_mm <= TOF_WALL_STOP;
}

/* Drive forward until wall */
int nav_drive_to_wall(void) {
    if (g_robot.tof_fwd_mm <= TOF_WALL_STOP) return 1;
    int8_t cm = 180;
    step(&cm);
    return 0;
}

/* Drive exactly dist_cm */
int nav_step_cm(int8_t dist_cm) {
    return step(&dist_cm);
}

/* Rotate to target_hdg degrees */
int nav_rotate_to(int16_t target_hdg) {
    int16_t ang = target_hdg - g_robot.heading;
    while (ang >  180) ang -= 360;
    while (ang < -180) ang += 360;

    if (ang > -HDG_THRESH && ang < HDG_THRESH) return 1;
    int8_t a = (int8_t)((ang > 127) ? 127 : (ang < -128) ? -128 : ang);
    return rotate(&a);
}

/* Drive west until past x_thresh (used for cave exit). */
int nav_drive_past_x(int16_t x_thresh) {
    if (g_robot.x < x_thresh) return 1;
    int8_t cm = 10;
    step(&cm);
    return 0;
}

/* Navigate to waypoint: Returns 1 when reached, 0 still navigating. */
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
        int16_t dx = (int16_t)wp->x-g_robot.x;
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

/* ══════════════════════════════════════════════════════════════
   MISSION HANDLERS
   ══════════════════════════════════════════════════════════════ */

Mission handle_wait_start(SM *s) {
    (void)s;
    if (g_robot.start_detected) return MISSION_LAWN_OPEN;
    return MISSION_WAIT_START;
}

/* Lawnmower sweep of open arena.
   sub_step 0: drive north/south until wall
   sub_step 1: turn east
   sub_step 2: step one row width east
   sub_step 3: flip row direction, back to sub_step 0           */

Mission handle_sweep_west(SM *s) {
    // turn -45 to read west tag for rendezvous point
    // turn to -90 to west wall
    // step until wall detected
    // turn to 0 degrees to north wall
    // step until wall detected
    // turn to 90 to east 
    // step toward container
    // return MISSION_GRAB_NEB

}

Mission handle_grab_neb(SM *s) {
    // turn 180
    // back up into container (hook should be going in hole)
    // lift up hooks to lift container
    // go to rendezvous point, algorithm changes depending on which side pad is on
    // drop hook and back out (or lateral step out)
    // return MISSION_GRAB_GEO
}

Mission handle_grab_geo(SM *s) {
    // from rendezvous point, go to just outside the drop off zone
    // drive south until wall
    // turn to 90 toward east
    // drive straight toward container
    // turn 180
    // back up into container (hook should be going in hole)
    // lift up hook s to lift container
    // go to rendezvous point, algorithm changes depending on which side pad is on
    // drop hook and back out (or lateral step out)
    // return MISSION_LAWN_OPEN
}

// should sweep the open arena using nearest unvisisted waypoint
Mission handle_lawn_open(SM *s) {
    if (nav_time_exceeded(DROPOFF_MS)) return MISSION_EXIT_CAVE;

    /* Stuck recovery — use camera hint to find best next waypoint */
    if (get_global_tick() - s->last_progress_tick > RECOVERY_MS) {
        int i = wp_nearest_with_cam_hint(s, WP_OPEN);
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
        /* Drive until motor stops at wall, then validate with ToF */
        if (nav_drive_to_wall()) {
            /* Hard-reset y from known wall position. */
            g_robot.y = (s->row_parity == 0) ? Y_NORTH : Y_SOUTH;
            s->last_progress_tick = get_global_tick();
            s->sub_step = 1;
        }
        return MISSION_LAWN_OPEN;

    case 1:
        if (nav_rotate_to(90)) s->sub_step = 2;
        return MISSION_LAWN_OPEN;

    case 2:
        if (nav_step_cm(ROW_STEP_CM)) s->sub_step = 3;
        return MISSION_LAWN_OPEN;

    case 3: {
        int16_t face = (s->row_parity == 0) ? 180 : 0;
        if (nav_rotate_to(face)) {
            s->row_parity ^= 1;
            s->sub_step = 0;
        }
        return MISSION_LAWN_OPEN;
    }

    }
    return MISSION_LAWN_OPEN;
}

Mission handle_wp_recover(SM *s) {
    if (wp_nav_to(s)) {
        s->sub_step           = 0;
        s->last_progress_tick = get_global_tick();
        return s->prev_mission;
    }
    return MISSION_WP_RECOVER;
}

Mission handle_cave_enter(SM *s) {
    switch (s->sub_step) {

    case 0:
        if (nav_rotate_to(180)) s->sub_step = 1;
        return MISSION_CAVE_ENTER;

    case 1:
        /* Drive south to wall, reset y from known south wall pos */
        if (nav_drive_to_wall()) {
            g_robot.y = Y_SOUTH;
            s->sub_step = 2;
        }
        return MISSION_CAVE_ENTER;

    case 2:
        if (nav_rotate_to(0)) s->sub_step = 3;
        return MISSION_CAVE_ENTER;

    case 3: {
        int16_t dy = Y_CAVE_CENTER - g_robot.y;
        if (dy < 0) dy = -dy;
        if (dy < DIST_THRESH) { s->sub_step = 4; return MISSION_CAVE_ENTER; }
        int8_t cm = (int8_t)((dy/48 > 120) ? 120 : dy/48);
        nav_step_cm(cm);
        return MISSION_CAVE_ENTER;
    }

    case 4:
        if (nav_rotate_to(90)) s->sub_step = 5;
        return MISSION_CAVE_ENTER;

    case 5:
        /* Drive east into cave. Use x position OR wall contact. */
        if (g_robot.x > X_CAVE_IN || nav_wall_ahead()) {
            s->in_cave    = 1;
            s->row_parity = 0;
            g_robot.x     = X_CAVE_IN;     /* hard-reset at known position */
            for (int i = 0; i < (int)NUM_WP; i++)
                if ((WpType)wps[i].type == WP_CAVE_ENTER) wps[i].visited = 1;
            s->sub_step = 0;
            return MISSION_LAWN_CAVE;
        }
        nav_step_cm(10);
        return MISSION_CAVE_ENTER;

    }
    return MISSION_CAVE_ENTER;
}

Mission handle_lawn_cave(SM *s) {
    if (nav_time_exceeded(DROPOFF_MS)) return MISSION_EXIT_CAVE;

    if (get_global_tick() - s->last_progress_tick > RECOVERY_MS) {
        int i = wp_nearest_with_cam_hint(s, WP_CAVE);
        if (i >= 0) {
            s->wp_target_idx = i;
            s->wp_nav_step   = 0;
            s->prev_mission  = MISSION_LAWN_CAVE;
            s->sub_step      = 0;
            return MISSION_WP_RECOVER;
        }
    }

    if (wp_all_done(WP_CAVE)) { s->sub_step = 0; return MISSION_EXIT_CAVE; }

    wp_check_visit(s);

    switch (s->sub_step) {

    case 0:
        if (nav_drive_to_wall()) {
            g_robot.y = (s->row_parity == 0) ? Y_NORTH : Y_SOUTH;
            s->last_progress_tick = get_global_tick();
            s->sub_step = 1;
        }
        return MISSION_LAWN_CAVE;

    case 1:
        if (nav_rotate_to(270)) s->sub_step = 2;
        return MISSION_LAWN_CAVE;

    case 2:
        /* Check if we've stepped back out of the cave */
        if (g_robot.x <= X_CAVE_THRESH) {
            s->in_cave  = 0;
            s->sub_step = 0;
            return MISSION_EXIT_CAVE;
        }
        if (nav_step_cm(ROW_STEP_CM)) s->sub_step = 3;
        return MISSION_LAWN_CAVE;

    case 3: {
        int16_t face = (s->row_parity == 0) ? 180 : 0;
        if (nav_rotate_to(face)) {
            s->row_parity ^= 1;
            s->sub_step = 0;
        }
        return MISSION_LAWN_CAVE;
    }

    }
    return MISSION_EXIT_CAVE;
}

Mission handle_exit_cave(SM *s) {
    switch (s->sub_step) {

    case 0:
        if (nav_rotate_to(270)) s->sub_step = 1;
        return MISSION_EXIT_CAVE;

    case 1:
        if (nav_drive_past_x(X_CAVE_THRESH)) {
            s->in_cave = 0;
            for (int i = 0; i < (int)NUM_WP; i++)
                if ((WpType)wps[i].type == WP_CAVE_EXIT) wps[i].visited = 1;
            s->sub_step = 0;
            return MISSION_DONE;
        }
        return MISSION_EXIT_CAVE;

    }
    return MISSION_DONE;
}

MISSION_DROPOFF(SM *s) {
    // whenever there is enough balls sorted, go to drop off (release the hopper walls)
    // algorithm for getting to dropoff point changes depending on where it is
    // return to previous mission
    // when all waypoints have been covered, do a drop off one final time
    // 
}

Mission handle_done(SM *s) {
    (void)s;
    turn_off_motors();
    return MISSION_DONE;
}

/* ══════════════════════════════════════════════════════════════
   DISPATCH TABLE
   ══════════════════════════════════════════════════════════════ */
typedef Mission (*MissionHandler)(SM *);

// TODO: containers, drop containers at rendezvous, sorting, drop bags
MissionHandler handlers[MISSION_COUNT] = {
    [MISSION_WAIT_START] = handle_wait_start,
    [MISSION_LAWN_OPEN]  = handle_lawn_open,
    [MISSION_WP_RECOVER] = handle_wp_recover,
    [MISSION_CAVE_ENTER] = handle_cave_enter,
    [MISSION_LAWN_CAVE]  = handle_lawn_cave,
    [MISSION_EXIT_CAVE]  = handle_exit_cave,
    [MISSION_DONE]       = handle_done,
};

/* ══════════════════════════════════════════════════════════════
   INIT AND TICK
   ══════════════════════════════════════════════════════════════ */
void sm_init(SM *s) {
    // s->mission            = MISSION_WAIT_START;
    // s->prev_mission       = MISSION_WAIT_START;
    s->mission = MISSION_LAWN_OPEN;
    s->prev_mission = MISSION_LAWN_OPEN;
    s->sub_step           = 0;
    s->wp_target_idx      = -1;
    s->wp_nav_step        = 0;
    s->row_parity         = 0;
    s->in_cave            = 0;
    s->last_progress_tick = get_global_tick();
}

void sm_tick(SM *s) {
    Mission next = handlers[s->mission](s);
    if (next != s->mission) {
        s->prev_mission = s->mission;
        s->mission      = next;
        s->sub_step     = 0;
    }
}

void sm_main(void *args) {
    (void)args;
    robot_state_init();         
    SM s;
    sm_init(&s);
    while (s.mission != MISSION_DONE) {
        robot_state_update();   
        sm_tick(&s);           
    }
    turn_off_motors();
    while (1) { block(); }
}
