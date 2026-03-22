#include "statemachine.h"
#include "motors.h"
#include "i2c.h"
#include "lock.h"
#include "tcb.h"
#include "lcd.h"
#include <stdint.h>

/* ══════════════════════════════════════════════════════════════
   UNITS AND FIELD GEOMETRY: raw units/48 = cm
   ══════════════════════════════════════════════════════════════ */

/* Wall positions (robot centre when flush against wall) */
#define X_WEST          15*48
#define X_EAST          221*48         
#define Y_SOUTH         15*48          
#define Y_NORTH         99*48          

// cave Thresholds
#define X_CAVE_THRESH   148*48
#define X_CAVE_IN       165*48
#define Y_CAVE_CENTER   57*48

#define ROW_STEP_CM     25

/* Start */
#define START_X         3810
#define START_Y         731
#define START_HDG       0           // heading

/* Thresholds */
#define TOF_WALL_STOP   200         // mm
#define DIST_THRESH     5*48
#define HDG_THRESH      5           // degrees    
#define VISITED_THRESH  15*48
#define RECOVERY_MS     4000
#define DROPOFF_MS      140000      // 2:20

/* Camera protocol */
#define CAM_FN_SEE_TAG  0x00
#define CAM_FN_ID       0x01
#define CAM_FN_SQRT     0x05
#define CAM_FN_PROJ_X   0x07
#define CAM_FN_PROJ_Y   0x08
#define CAM_FRONT       0x01
#define CAM_BACK        0x02
#define CAM_OFFSET 15*48    // will need to change to real offset

/* Container positions */
#define X_NEB           3200
#define X_GEO           6370

/* Rendezvous pad y-centres (from south wall) */
int16_t PAD_Y[5] = { 518, 1646, 2743, 3840, 4968 };

Vec2 TAG_WORLD[8]={
    {0,2743},
    {0,2743},
    {0,2743},
    {0,2743},
    {0,2743},
    {3885,5486},
    {5313,0},
    {11338,2743}
};

int16_t TAG_FACING[8]={270,270,270,270,270,0,180,90};

/* ══════════════════════════════════════════════════════════════
   WAYPOINTS 
   ══════════════════════════════════════════════════════════════ */

Waypoint wps[] = {
    /* Open arena row endpoints */
    { 15*48, 15*48, WP_OPEN, 0}, 
    {140*48, 15*48, WP_OPEN, 0},
    {140*48, 40*48, WP_OPEN, 0}, 
    { 15*48, 40*48, WP_OPEN, 0},
    { 15*48, 65*48, WP_OPEN, 0}, 
    {140*48, 65*48, WP_OPEN, 0},
    {140*48, 90*48, WP_OPEN, 0}, 
    { 15*48, 90*48, WP_OPEN, 0},
    /* Cave entry */
    {155*48, 57*48, WP_CAVE_ENTER, 0},
    {172*48, 57*48, WP_CAVE_ENTER, 0},
    /* Cave row endpoints */
    {172*48, 15*48, WP_CAVE, 0}, 
    {221*48, 15*48, WP_CAVE, 0},
    {221*48, 40*48, WP_CAVE, 0}, 
    {172*48, 40*48, WP_CAVE, 0},
    {172*48, 65*48, WP_CAVE, 0}, 
    {221*48, 65*48, WP_CAVE, 0},
    {221*48, 90*48, WP_CAVE, 0}, 
    {172*48, 90*48, WP_CAVE, 0},
    /* Cave exit */
    {155*48, 57*48, WP_CAVE_EXIT, 0},
};

#define NUM_WP (sizeof(wps)/sizeof(wps[0]))

/* MOTOR WRAPPERS */
// drive fwd cm
int drive_cm(int8_t dist_cm) {
    return step(&dist_cm);
}

int turn_to(Robot *r, int16_t target_hdg) {
    int16_t ang = target_hdg - r->heading;
    while(ang > 180) ang -= 360;
    while(ang < -180) ang += 360;
    if (ang > -HDG_THRESH && ang < HDG_THRESH) return 1;
    int8_t a = (int8_t)((ang > 127) ? 127 : (ang < -128) ? -128 : ang);
    return rotate(&a);
}

/* ══════════════════════════════════════════════════════════════
   CAMERA 
   ══════════════════════════════════════════════════════════════ */
uint8_t cam_req(uint8_t fn, uint8_t a1, uint8_t a2) {
    USART1->CR1 &= ~USART_RXNEIE;
    uint8_t p[5] = {0xAA, fn, a1, a2, 0x55};

    for(int i=0;i<5;i++){
        while(!(USART1->ISR&USART_TXE));
        USART1->TDR=p[i];
    }

    while(!(USART1->ISR & USART_TC)); 
    USART1->ICR |= (1<<6);
    uint8_t r[4] = {0};

    for(int i=0; i<4; i++){
        uint32_t t = 200000;
        while(!(USART1->ISR & USART_RXNE) && --t);
        if(!t){
            USART1->CR1 |= USART_RXNEIE;
            return 0xFF;
        } 
        r[i] = USART1->RDR;
    }

    USART1->CR1 |= USART_RXNEIE;
    return (r[0] == 0xAA && r[3] == 0x55) ? r[1] : 0xFF;
}

int16_t cam_req16(uint8_t fn, uint8_t a1) {
    USART1->CR1 &= ~USART_RXNEIE;
    uint8_t p[5] = {0xAA, fn, a1, 0, 0x55};
    for (int i = 0; i < 5; i++) {
        while (!(USART1->ISR & USART_TXE));
        USART1->TDR = p[i];
    }

    while (!(USART1->ISR & USART_TC));
    USART1->ICR |= (1 << 6);
    uint8_t r[4] = {0};
    for (int i = 0; i < 4; i++) {
        uint32_t t = 200000;
        while (!(USART1->ISR & USART_RXNE) && --t);
        if (!t) { USART1->CR1 |= USART_RXNEIE; return 0; }
        r[i] = USART1->RDR;
    }

    USART1->CR1 |= USART_RXNEIE;
    if (r[0] != 0xAA || r[3] != 0x55) return 0;
    return (int16_t)((uint16_t)r[1] | ((uint16_t)r[2] << 8));
}

/* ══════════════════════════════════════════════════════════════
   POSE CORRECTION
   ══════════════════════════════════════════════════════════════ */
void pose_correct(Robot *r, uint8_t side) {
    uint8_t id = r->cam_id[side == CAM_FRONT ? 0 : 1];
    if(id > 7) return; // sanity check

    int16_t px = cam_req16(CAM_FN_PROJ_X, side) * 48;
    int16_t py = cam_req16(CAM_FN_PROJ_Y, side) * 48;

    int16_t tx = TAG_WORLD[id].x; 
    int16_t ty = TAG_WORLD[id].y;

    int16_t cam_hdg = (side == CAM_BACK)
                    ? (r->heading + 180) % 360
                    : r->heading;

    int s_t[4]={0,1,0,-1};
    int c_t[4]={1,0,-1,0};

    int idx = ((cam_hdg + 45) / 90) % 4;

    int16_t ex = tx - (px*c_t[idx] - py*s_t[idx]) - (CAM_OFFSET*s_t[idx]);
    int16_t ey = ty - (px*s_t[idx] + py*c_t[idx]) - (CAM_OFFSET*c_t[idx]);
    
    // blend
    r->x = (int16_t)(((int32_t)r->x*7 + (int32_t)ex*3) / 10);
    r->y = (int16_t)(((int32_t)r->y*7 + (int32_t)ey*3) / 10);
}

/* ══════════════════════════════════════════════════════════════
   WAYPOINT HELPERS
   ══════════════════════════════════════════════════════════════ */
// returns distance from waypoint
uint16_t wp_dist(Robot *r, const Waypoint *w) {
    int16_t dx = (r->x - w->x) / 48;   // to cm
    int16_t dy = (r->y - w->y) / 48;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    uint32_t sum = (uint32_t)dx*dx + (uint32_t)dy*dy;
    uint8_t lo = (uint8_t)(sum & 0xFF);
    uint8_t hi = (uint8_t)((sum >> 8) & 0xFF);
    uint8_t d_cm = cam_req(CAM_FN_SQRT,lo,hi);

    return (uint16_t)d_cm * 48;
}

void wp_check_visit(Robot *r) {
    for(int i = 0; i < NUM_WP; i++){
        uint16_t d = wp_dist(r, &wps[i]);
        if(!wps[i].visited && (d <= VISITED_THRESH)) {
            wps[i].visited = 1;  
        }
    } 
}

int wp_nearest(Robot *r, WpType t) {
    int nearest = -1; 
    uint16_t shortest = 0xFFFF;

    for(int i = 0; i < NUM_WP; i++){
        if(!wps[i].visited && wps[i].type == t) {
            uint16_t d = wp_dist(r,&wps[i]);
            if(d < shortest){
                shortest = d;
                nearest = i;
            }
        }
    }

    return nearest;
}

int wp_all_done(WpType t) {
    for(int i = 0; i < NUM_WP; i++){
        if(!wps[i].visited && (WpType)wps[i].type == t){
            return 0;
        }
    }
    return 1;
}

// return 1 when done, 0 still navigating
int wp_nav_to(Robot *r){
    if(r->wp_target_idx < 0) return 1; // sanity check
                                       
    Waypoint *wp = &wps[r->wp_target_idx];

    switch(r->wp_nav_step){
    case 0:
        if(!turn_to(r, (r->x < wp->x) ? 90 : 270)) return 0;
        r->wp_nav_step = 1; 
        return 0;

    case 1: {

        int16_t dx = wp->x - r->x; 
        if(dx < 0) dx = -dx;
        if(dx < DIST_THRESH) {
            r->wp_nav_step = 2;
            return 0;
        }
        int8_t cm = (int8_t)((dx/48 > 120) ? 120 : dx/48);
        drive_cm(cm);
        return 0;
    }

    case 2:
        if(!turn_to(r,(r->y < wp->y) ? 0 : 180)) return 0;
        r->wp_nav_step = 3; 
        return 0;

    case 3: {
        int16_t dy = wp->y - r->y; 
        if(dy < 0) dy = -dy;
        if(dy < DIST_THRESH) {
            wps[r->wp_target_idx].visited = 1;
            r->last_progress_tick = get_global_tick();
            r->wp_nav_step = 0; 
            r->wp_target_idx = -1; 
            return 1;
        }
        int8_t cm = (int8_t)((dy/48 > 120) ? 120 : dy/48);
        drive_cm(cm);
        return 0;
    } 
    }
    return 1;
}

/* ══════════════════════════════════════════════════════════════
   SENSOR UPDATE 
   ══════════════════════════════════════════════════════════════ */
void sensor_update(Robot *r){
    // gyroscope heading
    r->heading = (i2c_rx_buffer[HEADING_MSB]<<8 | i2c_rx_buffer[HEADING_LSB])/16;
    r->elapsed_ms = get_global_tick() - r->start_tick;
    r->tof_fwd_mm = get_tof_distance();
    
    // encoder dead-reckoning

    {
        int16_t delta_cm = get_encoder_delta_cm();
        if (delta_cm != 0) {
            /* sin/cos for 0=N,90=E,180=S,270=W */
            int8_t cx[4] = {  0,  1,  0, -1 };
            int8_t cy[4] = {  1,  0, -1,  0 };
            int idx = ((r->heading + 45) / 90) % 4;
            r->x += (int16_t)((int32_t)delta_cm * 48 * cx[idx]);
            r->y += (int16_t)((int32_t)delta_cm * 48 * cy[idx]);
        }
    }

    r->cam_sees = cam_req(CAM_FN_SEE_TAG,0,0);

    if(r->cam_sees & 0x01){
        r->cam_id[0] = cam_req(CAM_FN_ID,CAM_FRONT,0);
        pose_correct(r,CAM_FRONT);
        if(r->cam_id[0] <= 4) r->telemetry_pad = r->cam_id[0];
    } else {
        r->cam_id[0] = 0xFF;
    }

    if(r->cam_sees & 0x02){
        r->cam_id[1] = cam_req(CAM_FN_ID,CAM_BACK,0);
        pose_correct(r,CAM_BACK);
        if(r->cam_id[1] <= 4) r->telemetry_pad = r->cam_id[1];
    } else {
        r->cam_id[1] = 0xFF;
    }

    if(r->mission == MISSION_LAWN_OPEN || r->mission == MISSION_LAWN_CAVE)
        wp_check_visit(r);
}

/* ══════════════════════════════════════════════════════════════
   INIT
   ══════════════════════════════════════════════════════════════ */
void robot_init(Robot *r){
    r->mission = MISSION_LAWN_OPEN; 
    r->prev_mission = MISSION_LAWN_OPEN; 
    r->x = START_X; 
    r->y = START_Y; 
    r->heading = START_HDG;
    r->tof_fwd_mm = 9999;
    r->cam_sees = 0; 
    r->cam_id[0] = 0xFF; 
    r->cam_id[1] = 0xFF;
    r->telemetry_pad = 2;
    r->sub_step = 0;
    r->wp_nav_step = 0;
    r->row_parity = 0;
    r->in_cave = 0;
    r->wp_target_idx = -1; 
    r->start_tick = get_global_tick(); 
    r->elapsed_ms = 0;
    r->last_progress_tick = r->start_tick;
}

/* ══════════════════════════════════════════════════════════════
   MISSION HANDLERS
   ══════════════════════════════════════════════════════════════ */

Mission handle_lawn_open(Robot *r){
    if(r->elapsed_ms >= DROPOFF_MS) return MISSION_EXIT_CAVE;
    
    // nav to nearest open waypoint
    if(get_global_tick() - r->last_progress_tick > RECOVERY_MS){
        int i = wp_nearest(r,WP_OPEN);
        if(i >= 0){
            r->wp_target_idx = i;
            r->wp_nav_step = 0;
            r->prev_mission = MISSION_LAWN_OPEN;
            r->sub_step = 0;
            return MISSION_WP_RECOVER;
        }
    }

    if(wp_all_done(WP_OPEN)){
        r->sub_step = 0; 
        return MISSION_CAVE_ENTER;
    }

    switch(r->sub_step){
        
    case 0: 
        // drive until wall
        if(r->tof_fwd_mm <= TOF_WALL_STOP){
            // reset y from wall contact
            r->y = (r->row_parity==0) ? Y_NORTH : Y_SOUTH;
            r->last_progress_tick = get_global_tick();
            r->sub_step = 1;
            return MISSION_LAWN_OPEN;
        }
        drive_cm(10);
        return MISSION_LAWN_OPEN;

    case 1: 
        // turn east
        if(!turn_to(r,90)) return MISSION_LAWN_OPEN; 
        r->sub_step = 2; 
        return MISSION_LAWN_OPEN;

    case 2:
        // advance one row east
        if(drive_cm(ROW_STEP_CM)) r->sub_step = 3;
        return MISSION_LAWN_OPEN;

    case 3: {
        // flip direction and start next row
        int16_t face = (r->row_parity == 0) ? 180: 0;
        if(!turn_to(r,face)) return MISSION_LAWN_OPEN;
        r->row_parity ^= 1;
        r->sub_step = 0;
        return MISSION_LAWN_OPEN;
    }
    }
    return MISSION_LAWN_OPEN;
}

/* WP_RECOVER */
Mission handle_wp_recover(Robot *r){
    if(wp_nav_to(r)) {
        r->sub_step = 0;
        r->last_progress_tick = get_global_tick();
        return r->prev_mission;
    }
    return MISSION_WP_RECOVER;
}

Mission handle_cave_enter(Robot *r){
    switch(r->sub_step){
    case 0: /* hit south wall: get y exact */
        if(!turn_to(r,180)) return MISSION_CAVE_ENTER; 
        r->sub_step=1; 
        return MISSION_CAVE_ENTER;

    case 1: 
        if(r->tof_fwd_mm <= TOF_WALL_STOP){
            r->y = Y_SOUTH;
            r->sub_step = 2;
            return MISSION_CAVE_ENTER;
        }
        drive_cm(10);
        return MISSION_CAVE_ENTER;

    case 2: /* step count north to cave centre y */
        if(!turn_to(r,0)) return MISSION_CAVE_ENTER;
        r->sub_step=3; 
        return MISSION_CAVE_ENTER;

    case 3: {
        int16_t dy = Y_CAVE_CENTER - r->y;
        if (dy < 0) dy = -dy;
        if (dy < DIST_THRESH) {
            r->sub_step = 4;
            return MISSION_CAVE_ENTER;
        }
        int8_t cm = (int8_t)((dy/48 > 120) ? 120 : dy/48);
        drive_cm(cm);
        return MISSION_CAVE_ENTER;
    }

    case 4: /* face east, drive into cave */
        if(!turn_to(r,90)) return MISSION_CAVE_ENTER; 
        r->sub_step = 5; 
        return MISSION_CAVE_ENTER;

    case 5: 
        if(r->x > X_CAVE_IN || r->tof_fwd_mm <= TOF_WALL_STOP){
            r->in_cave = 1;
            r->row_parity = 0;
            for(int i = 0; i < NUM_WP; i++){
                if(wps[i].type == WP_CAVE_ENTER) wps[i].visited = 1;
            }
            r->sub_step = 0;
            return MISSION_LAWN_CAVE;
        }
        drive_cm(10);
        return MISSION_CAVE_ENTER;
    } 
    return MISSION_CAVE_ENTER;
}

// same as LAWN_OPEN but advances west
Mission handle_lawn_cave(Robot *r){
    if(r->elapsed_ms >= DROPOFF_MS) return MISSION_EXIT_CAVE;

    if(get_global_tick() - r->last_progress_tick > RECOVERY_MS){
        int i = wp_nearest(r,WP_CAVE);
        if(i >= 0){
            r->wp_target_idx = i;
            r->wp_nav_step = 0;
            r->prev_mission = MISSION_LAWN_CAVE;
            r->sub_step = 0;
            return MISSION_WP_RECOVER;
        }
    }

    if(wp_all_done(WP_CAVE)){r->sub_step = 0; return MISSION_EXIT_CAVE;}

    switch(r->sub_step){
    case 0: 
        // drive until wall
        if(r->tof_fwd_mm <= TOF_WALL_STOP){
            // reset y from wall contact
            if(r->row_parity==0) r->y = Y_NORTH;
            else r->y = Y_SOUTH;
            r->last_progress_tick = get_global_tick();
            r->sub_step = 1;
            return MISSION_LAWN_CAVE;
        }
        drive_cm(10);
        return MISSION_LAWN_CAVE;

    case 1: 
        // turn east
        if(!turn_to(r,270)) return MISSION_LAWN_CAVE; 
        r->sub_step = 2; 
        return MISSION_LAWN_CAVE;

    case 2:
        if(r->x <= X_CAVE_THRESH){
            r->in_cave = 0;
            r->sub_step = 0;
            return MISSION_EXIT_CAVE;
        }
        drive_cm(ROW_STEP_CM);
        r->sub_step = 3;
        return MISSION_LAWN_CAVE;

    case 3: {
        // flip direction and start next row
        int16_t face = (r->row_parity == 0) ? 180: 0;
        if(!turn_to(r,face)) return MISSION_LAWN_CAVE;
        r->row_parity ^= 1;
        r->sub_step = 0;
        return MISSION_LAWN_CAVE;
    }
    }
    return MISSION_EXIT_CAVE;
}

/* EXIT_CAVE: drive west past threshold, mark exit WP visited */
Mission handle_exit_cave(Robot *r){
    switch(r->sub_step){
    case 0: 
        if(!turn_to(r,270))return MISSION_EXIT_CAVE; 
        r->sub_step=1; 
        return MISSION_EXIT_CAVE;

    case 1: 
        if(r->x < X_CAVE_THRESH || r->tof_fwd_mm <= TOF_WALL_STOP) {
            r->in_cave = 0;
            for(int i = 0; i < NUM_WP; i++)
                if(wps[i].type == WP_CAVE_EXIT) wps[i].visited = 1;
            r->sub_step = 0; 
            return MISSION_DONE;
        }
        drive_cm(10);
        return MISSION_EXIT_CAVE;
    } 
    return MISSION_DONE;
}

Mission handle_done(Robot *r){
    (void)r;
    turn_off_motors();
    return MISSION_DONE;
}

/* ══════════════════════════════════════════════════════════════
   DISPATCH TABLE
   ══════════════════════════════════════════════════════════════ */
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
