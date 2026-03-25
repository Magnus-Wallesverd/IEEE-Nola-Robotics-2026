#include "statemachine.h"
#include "robot_state.h"
#include "motors.h"
#include "lock.h"
#include "tcb.h"
#include <stdint.h>

// Endyne Units: raw / 48 = cm
// Heading: 0=N 90=E 180=S 270=W

// Field walls robot center position when touching the wall
#define X_WEST  (15 * 48)
#define X_EAST  (221 * 48)
#define Y_SOUTH (15 * 48)
#define Y_NORTH (99 * 48)

// Cave thresholds
#define X_CAVE_THRESH   (148 * 48)  // east of this is inside cave
#define X_CAVE_IN       (165 * 48)  // confirmed inside once past this
#define Y_CAVE_CENTER   (57 * 48)   // y to aim for when entering cave

// Lawnmower row spacing: about 10 inches between rows
#define ROW_STEP_CM 25

// x is always near the west wall, y depends on which pad
#define PAD_X (20 * 48)
const int16_t PAD_Y[5] = {518, 1646, 2743, 3840, 4968};

// Container positions
#define NEB_BOX_X (66 * 48)   // nebulite container
#define NEB_BOX_Y (90 * 48)
#define GEO_BOX_X (126 * 48)  // geodinium container
#define GEO_BOX_Y (15 * 48)

// Navigation thresholds
#define HDG_THRESH      5           // close enough to target heading
#define DIST_THRESH     (5  * 48)   // close enough to waypoint
#define VISITED_THRESH  (15 * 48)   // mark waypoint as visited
#define TOF_WALL_STOP   200         // this close to wall

// Timing
#define RECOVERY_MS     4000        // if stuck this long, navigate to nearest waypoint
#define DROPOFF_MS      140000      // 2:20 -> stop collecting and drop off
#define BALLS_TO_DROPOFF 8          // drop off when we have this many balls

// Waypoints (entire game field)
Waypoint wps[] = {
    // open arena 
    { 15*48, 15*48, WP_OPEN, 0 },
    { 140*48, 15*48, WP_OPEN, 0 },
    { 140*48, 40*48, WP_OPEN, 0 },
    { 15*48, 40*48, WP_OPEN, 0 },
    { 15*48, 65*48, WP_OPEN, 0 },
    { 140*48, 65*48, WP_OPEN, 0 },
    { 140*48, 90*48, WP_OPEN, 0 },
    { 15*48, 90*48, WP_OPEN, 0 },
    // cave entry
    { 155*48, 57*48, WP_CAVE_ENTER, 0 },
    { 172*48, 57*48, WP_CAVE_ENTER, 0 },
    // inside cave
    { 172*48, 15*48, WP_CAVE, 0 },
    { 221*48, 15*48, WP_CAVE, 0 },
    { 221*48, 40*48, WP_CAVE, 0 },
    { 172*48, 40*48, WP_CAVE, 0 },
    { 172*48, 65*48, WP_CAVE, 0 },
    { 221*48, 65*48, WP_CAVE, 0 },
    { 221*48, 90*48, WP_CAVE, 0 },
    { 172*48, 90*48, WP_CAVE, 0 },
    // cave exit
    { 155*48,  57*48, WP_CAVE_EXIT, 0 },
};

#define NUM_WP (sizeof(wps) / sizeof(wps[0]))


/* ---------- WAYPOINT HELPERS ---------- */

// Manhattan distance from robot to a waypoint
uint16_t wp_dist(Waypoint *w) {
    int16_t dx = (g_robot.x - (int16_t)w->x) / 48; // to cm
    int16_t dy = (g_robot.y - (int16_t)w->y) / 48;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return (uint16_t)(dx + dy) * 48;
}

// mark waypoints as visited if robot passes through it
void wp_check_visit(SM *s) {
    (void)s;
    for (int i = 0; i < (int)NUM_WP; i++) {
        if (!wps[i].visited && wp_dist(&wps[i]) <= VISITED_THRESH)
            wps[i].visited = 1;
    }
}

// find nearest unvisited waypoint of a given type
int wp_nearest(SM *s, WpType t) {
    (void)s;
    int nearest  = -1;
    uint16_t shortest = 0xFFFF;

    for (int i = 0; i < (int)NUM_WP; i++) {
        if (!wps[i].visited && (WpType)wps[i].type == t) {
            int32_t dx = (int32_t)wps[i].x - g_robot.x;
            int32_t dy = (int32_t)wps[i].y - g_robot.y;

            if (dx < 0) dx = -dx;
            if (dy < 0) dy = -dy;

            uint16_t d = (uint16_t)(dx/48 + dy/48); // in cm

            if (d < shortest) { 
                shortest = d; 
                nearest = i; 
            }
        }
    }
    return nearest;
}

// returns 1 if all waypoints of a given type have been visited
int wp_all_done(WpType t) {
    for (int i = 0; i < (int)NUM_WP; i++) {
        if (!wps[i].visited && (WpType)wps[i].type == t) return 0;
    }
    return 1;
}

// navigate through waypoint array
// returns 1 if arrived
int wp_nav_to(SM *s) {
    if (s->wp_target_idx < 0) return 1; // sanity check
    Waypoint *wp = &wps[s->wp_target_idx];

    switch (s->wp_nav_step) {

    case 0:  // face east/west direction
        if (!nav_rotate_to(g_robot.x < (int16_t)wp->x ? 90 : 270)) return 0;
        s->wp_nav_step = 1;
        return 0;

    case 1: {  // drive east/west until close enough
        int16_t dx = (int16_t)wp->x - g_robot.x;
        if (dx < 0) dx = -dx;

        if (dx < DIST_THRESH) { 
            s->wp_nav_step = 2; 
            return 0; 
        }

        int8_t cm = (int8_t)((dx/48 > 120) ? 120 : dx/48); // limit dist to 120 cm
        nav_step_cm(cm);
        return 0;
    }

    case 2:  // face the right north/south direction
        if (!nav_rotate_to(g_robot.y < (int16_t)wp->y ? 0 : 180)) return 0;
        s->wp_nav_step = 3;
        return 0;

    case 3: {  // drive north/south until close enough
        int16_t dy = (int16_t)wp->y - g_robot.y;
        if (dy < 0) dy = -dy;
        if (dy < DIST_THRESH) {
            wps[s->wp_target_idx].visited = 1;
            s->last_progress_tick = get_global_tick();
            s->wp_nav_step   = 0;
            s->wp_target_idx = -1;
            return 1;
        }
        int8_t cm = (int8_t)((dy/48 > 120) ? 120 : dy/48); // limit dist to 120 cm
        nav_step_cm(cm);
        return 0;
    }

    }
    return 1;
}


/* ---------- NAVIGATION HELPERS ---------- */
// check time
int nav_time_exceeded(uint32_t ms) {
    return g_robot.elapsed_ms >= ms;
}

// check if there's a wall close in front
int nav_wall_ahead(void) {
    return g_robot.tof_mm <= TOF_WALL_STOP;
}

// drive foward until ToF is close enough
int nav_drive_to_wall(void) {
    if (g_robot.tof_mm <= TOF_WALL_STOP) return 1;
    int8_t cm = 10;
    step(&cm);
    return 0;
}

// drive exactly this many cm
int nav_step_cm(int8_t dist_cm) {
    return step(&dist_cm);
}

// rotate to a target heading
int nav_rotate_to(int16_t target_hdg) {
    int16_t ang = target_hdg - g_robot.heading;
    while (ang >  180) ang -= 360;
    while (ang < -180) ang += 360;
    if (ang > -HDG_THRESH && ang < HDG_THRESH) return 1;
    int8_t a = (int8_t)((ang > 127) ? 127 : (ang < -128) ? -128 : ang);
    return rotate(&a);
}

// drive west until passed x threshold
int nav_drive_past_x(int16_t x_thresh) {
    if (g_robot.x < x_thresh) return 1;
    int8_t cm = 10;
    step(&cm);
    return 0;
}

// drive toward a specific x,y target
// returns 1 when close enough
int nav_drive_to_xy(int16_t tx, int16_t ty) {
    int16_t dx = tx - g_robot.x;
    int16_t dy = ty - g_robot.y;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    if (dx < DIST_THRESH && dy < DIST_THRESH) return 1;

    // drive x first, then y
    if (dx >= DIST_THRESH) {
        nav_rotate_to(tx > g_robot.x ? 90 : 270);
        int8_t cm = (int8_t)((dx/48 > 120) ? 120 : dx/48);
        nav_step_cm(cm);
    } else {
        nav_rotate_to(ty > g_robot.y ? 0 : 180);
        int8_t cm = (int8_t)((dy/48 > 120) ? 120 : dy/48);
        nav_step_cm(cm);
    }

    return 0;
}


/* ---------- MISSION HANDLERS ---------- */ 
// returns the next mission, returns itself to stay.

// wait until rear camera sees start LED light up
Mission handle_wait_start(SM *s) {
    (void)s;
    if (g_robot.start_detected) return MISSION_SWEEP_WEST;
    return MISSION_WAIT_START;
}

// read rendezvous tag ID, then sweep the dropoff area
Mission handle_sweep_west(SM *s) {
    switch (s->sub_step) {
    case 0:  // turn toward and read rendezvous ID
        if (nav_rotate_to(300)) s->sub_step = 1;
        return MISSION_SWEEP_WEST;
    case 1:  // turn west
        if(nav_rotate_to(270)) s->sub_step = 2;
        return MISSION_SWEEP_WEST;
    case 2: // drive to west wall
        if(nav_drive_to_wall()) s->sub_step = 3;
        return MISSION_SWEEP_WEST;
    case 3: // turn north 
        if(nav_rotate_to(0)) s->sub_step = 4;
        return MISSION_SWEEP_WEST;
    case 4: // drive to north wall
        if(nav_drive_to_wall()) s->sub_step = 0;
        return MISSION_GRAB_NEB;
    }
    return MISSION_GRAB_NEB;
}

// pick up the nebulite container and bring it to the rendezvous pad
Mission handle_grab_neb(SM *s) {
    int16_t pad_y = PAD_Y[g_robot.telemetry_pad];

    switch (s->sub_step) {
    case 0:  // drive to nebulite container
        if (nav_drive_to_xy(NEB_BOX_X, NEB_BOX_Y)) s->sub_step = 1;
        return MISSION_GRAB_NEB;
    case 1:  // face west to back into container
        if (nav_rotate_to(270)) s->sub_step = 2;
        return MISSION_GRAB_NEB;
    case 2:  // back up until hook can grab
        {
            int8_t cm = -5;
            if (nav_wall_ahead()) s->sub_step = 3;
            else step(&cm);
        }
        return MISSION_GRAB_NEB;
    case 3:  // drive to rendezvous pad with container
        // TODO: raise hook servo 
        if (nav_drive_to_xy(PAD_X, pad_y)) s->sub_step = 4;
        return MISSION_GRAB_NEB;
    case 4:  // release container at pad
        // TODO: lower hook servo
        s->sub_step = 0;
        return MISSION_GRAB_GEO;
    }
    return MISSION_GRAB_GEO;
}

// pick up the geodinium container and bring it to the rendezvous pad
Mission handle_grab_geo(SM *s) {
    int16_t pad_y = PAD_Y[g_robot.telemetry_pad];

    switch (s->sub_step) {
    case 0:  // drive to geodinium container
        if (nav_drive_to_xy(GEO_BOX_X, GEO_BOX_Y)) s->sub_step = 1;
        return MISSION_GRAB_GEO;
    case 1:  // face west to back into container
        if (nav_rotate_to(270)) s->sub_step = 2;
        return MISSION_GRAB_GEO;
    case 2:  // back up until hook engages
        {
            int8_t cm = -5;
            if (nav_wall_ahead()) s->sub_step = 3;
            else step(&cm);
        }
        return MISSION_GRAB_GEO;
    case 3:  // drive to rendezvous pad
        // TODO: raise hook servo
        if (nav_drive_to_xy(PAD_X, pad_y)) s->sub_step = 4;
        return MISSION_GRAB_GEO;
    case 4:  // release container
        // TODO: lower hook servo
        s->sub_step = 0;
        return MISSION_LAWN_OPEN;
    }
    return MISSION_LAWN_OPEN;
}

// lawnmower sweep of open area
Mission handle_lawn_open(SM *s) {
    // time is almost up -> dropoff
    if (nav_time_exceeded(DROPOFF_MS)) {
        s->sub_step = 0;
        return MISSION_DROPOFF;
    }

//    // if hopper is full enough, detour to drop off then come back
//    if ((g_robot.count_geo + g_robot.count_neb) >= BALLS_TO_DROPOFF) {
//        s->dropoff_needed = 1;
//        s->sub_step = 0;
//        return MISSION_DROPOFF;
//    }

    // stuck -> nav to nearest waypoint
    if (get_global_tick() - s->last_progress_tick > RECOVERY_MS) {
        int i = wp_nearest(s, WP_OPEN);
        if (i >= 0) {
            s->wp_target_idx = i;
            s->wp_nav_step   = 0;
            s->prev_mission  = MISSION_LAWN_OPEN;
            s->sub_step      = 0;
            return MISSION_WP_RECOVER;
        }
    }

    if (wp_all_done(WP_OPEN)) {
        s->sub_step = 0;
        return MISSION_CAVE_ENTER;
    }

    wp_check_visit(s);

    switch (s->sub_step) {
    case 0:  // drive forward to wall
        if (nav_drive_to_wall()) {
            // wall hit -> hard reset y to known wall position
            g_robot.y = (s->row_parity == 0) ? Y_NORTH : Y_SOUTH;
            s->last_progress_tick = get_global_tick();
            s->sub_step = 1;
        }
        return MISSION_LAWN_OPEN;
    case 1:  // turn east to advance to next row
        if (nav_rotate_to(90)) s->sub_step = 2;
        return MISSION_LAWN_OPEN;
    case 2:  // step one row width east (~10 inches)
        if (nav_step_cm(ROW_STEP_CM)) s->sub_step = 3;
        return MISSION_LAWN_OPEN;
    case 3: {  // turn to face the opposite direction for next row
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

// nav to a waypoint when stuck or lost
Mission handle_wp_recover(SM *s) {
    if (wp_nav_to(s)) {
        s->sub_step           = 0;
        s->last_progress_tick = get_global_tick();
        return s->prev_mission;
    }
    return MISSION_WP_RECOVER;
}

// align to the cave entrance and drive in
Mission handle_cave_enter(SM *s) {
    switch (s->sub_step) {

    case 0:  // face south
        if (nav_rotate_to(180)) s->sub_step = 1;
        return MISSION_CAVE_ENTER;

    case 1:  // drive to south wall to get exact y position
        if (nav_drive_to_wall()) {
            g_robot.y = Y_SOUTH; 
            s->sub_step = 2;
        }
        return MISSION_CAVE_ENTER;

    case 2:  // face north
        if (nav_rotate_to(0)) s->sub_step = 3;
        return MISSION_CAVE_ENTER;

    case 3: {  // step north to cave entrance height
        int16_t dy = Y_CAVE_CENTER - g_robot.y;
        if (dy < 0) dy = -dy;
        if (dy < DIST_THRESH) { s->sub_step = 4; return MISSION_CAVE_ENTER; }
        int8_t cm = (int8_t)((dy/48 > 120) ? 120 : dy/48); // limit distance to 120cm
        nav_step_cm(cm);
        return MISSION_CAVE_ENTER;
    }

    case 4:  // face east toward cave
        if (nav_rotate_to(90)) s->sub_step = 5;
        return MISSION_CAVE_ENTER;

    case 5:  // drive into cave, turn on LED headlights
        if (g_robot.x > X_CAVE_IN || nav_wall_ahead()) {
            s->in_cave    = 1;
            s->row_parity = 0;
            g_robot.x     = X_CAVE_IN;  // hard reset x
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

// lawnmower inside cave
Mission handle_lawn_cave(SM *s) {
    if (nav_time_exceeded(DROPOFF_MS)) {
        s->sub_step = 0;
        return MISSION_DROPOFF;
    }

//    if ((g_robot.count_geo + g_robot.count_neb) >= BALLS_TO_DROPOFF) {
//        s->dropoff_needed = 1;
//        s->sub_step = 0;
//        return MISSION_DROPOFF;
//    }

    if (get_global_tick() - s->last_progress_tick > RECOVERY_MS) {
        int i = wp_nearest(s, WP_CAVE);
        if (i >= 0) {
            s->wp_target_idx = i;
            s->wp_nav_step   = 0;
            s->prev_mission  = MISSION_LAWN_CAVE;
            s->sub_step      = 0;
            return MISSION_WP_RECOVER;
        }
    }

    if (wp_all_done(WP_CAVE)) {
        s->sub_step = 0;
        return MISSION_EXIT_CAVE;
    }

    wp_check_visit(s);

    switch (s->sub_step) {
    case 0:  // drive until wall
        if (nav_drive_to_wall()) {
            g_robot.y = (s->row_parity == 0) ? Y_NORTH : Y_SOUTH;
            s->last_progress_tick = get_global_tick();
            s->sub_step = 1;
        }
        return MISSION_LAWN_CAVE;
    case 1:  // turn west to advance toward cave exit
        if (nav_rotate_to(270)) s->sub_step = 2;
        return MISSION_LAWN_CAVE;
    case 2:  // step one row width west
        if (g_robot.x <= X_CAVE_THRESH) {
            // crossed back out of the cave
            s->in_cave  = 0;
            s->sub_step = 0;
            return MISSION_EXIT_CAVE;
        }
        if (nav_step_cm(ROW_STEP_CM)) s->sub_step = 3;
        return MISSION_LAWN_CAVE;
    case 3: {  // flip row direction
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

// exit cave
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
            // dropoff balls, otherwise done
//            if (g_robot.count_geo + g_robot.count_neb > 0)
//                return MISSION_DROPOFF;
            return MISSION_DONE;
        }
        return MISSION_EXIT_CAVE;

    }
    return MISSION_DONE;
}

// drive to rendezvous pad and open hopper servo gates
// after dropping off, return to whatever previous mission was
Mission handle_dropoff(SM *s) {
    int16_t pad_y = PAD_Y[g_robot.telemetry_pad];

    switch (s->sub_step) {
    case 0:  // drive to rendezvous pad
        if (nav_drive_to_xy(PAD_X, pad_y)) s->sub_step = 1;
        return MISSION_DROPOFF;
    case 1:
        // TODO: open hopper servo gates here when servo code is ready
        // release all sorted balls into the containers sitting on the pad
        g_robot.count_geo = 0;
        g_robot.count_neb = 0;
        s->sub_step = 2;
        return MISSION_DROPOFF;
    case 2:
        // if all sweeping is done, we're done
        if (wp_all_done(WP_OPEN) && wp_all_done(WP_CAVE)) {
            s->sub_step = 0;
            return MISSION_DONE;
        }
        // otherwise go back to what we were doing
        s->dropoff_needed = 0;
        s->sub_step = 0;
        return s->prev_mission;
    }
    return MISSION_DONE;
}

Mission handle_done(SM *s) {
    (void)s;
    turn_off_motors();
    return MISSION_DONE;
}


/* ---------- DISPATCH TABLE ---------- */
typedef Mission (*MissionHandler)(SM *);

MissionHandler handlers[MISSION_COUNT] = {
    [MISSION_WAIT_START] = handle_wait_start,
    [MISSION_SWEEP_WEST] = handle_sweep_west,
    [MISSION_GRAB_NEB]   = handle_grab_neb,
    [MISSION_GRAB_GEO]   = handle_grab_geo,
    [MISSION_LAWN_OPEN]  = handle_lawn_open,
    [MISSION_WP_RECOVER] = handle_wp_recover,
    [MISSION_CAVE_ENTER] = handle_cave_enter,
    [MISSION_LAWN_CAVE]  = handle_lawn_cave,
    [MISSION_EXIT_CAVE]  = handle_exit_cave,
    [MISSION_DROPOFF]    = handle_dropoff,
    [MISSION_DONE]       = handle_done,
};


/* ---------- INIT & TICK ---------- */

void sm_init(SM *s) {
    s->mission            = MISSION_WAIT_START;
    s->prev_mission       = MISSION_WAIT_START;
    s->sub_step           = 0;
    s->wp_target_idx      = -1;
    s->wp_nav_step        = 0;
    s->row_parity         = 0;
    s->in_cave            = 0;
    s->dropoff_needed     = 0;
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
