#include "statemachine.h"
#include "motors.h"
#include "i2c.h"
#include "lock.h"
#include "tcb.h"
#include "lcd.h"
#include <stdint.h>

/* ══════════════════════════════════════════════════════════════
   FIELD DATA — coordinates in jiawei's units (÷48 = cm)
   ══════════════════════════════════════════════════════════════ */

const Vec2 TAG_WORLD[8] = {
    [0] = {0,2743},
    [1] = {0,2743},
    [2] = {0,2743},
    [3] = {0,2743},
    [4] = {0,2743},
    [5] = {3885,5486},
    [6] = {5313,0},
    [7] = {11338,2743}
};

const Vec2 RENDEZVOUS[5] = {
    [0] = {731,518},
    [1] = {731,1646},
    [2] = {731,2743},
    [3] = {731,3840},
    [4] = {731,4968}
};

const Vec2 WAYPOINTS[] = {
    {1097,1097},    // perimeter outside cave
    {1097,4389},
    {5638,4389},
    {5638,1097},

    {1097,2194},    // lawnmower outside cave
    {5638,2194},
    {5638,3353},
    {1097,3353},
    {1097,1097},
    {5638,1097},

    {5638,2743},    // going into cave
    {8625,2743},

    {8625,1097},    // perimeter in cave
    {10241,1097},
    {10241,4389},
    {8625,4389},
    {8625,2743},

    {10241,2743},   // lawnmower in cave
    {10241,1097},
    {8625,1097},
    {8625,4389},
    {10241,4389},
    {10241,2743}
};

#define NUM_WP (sizeof(WAYPOINTS)/sizeof(WAYPOINTS[0]))

#define START_X     3810
#define START_Y     731
#define START_THETA 90

Vec2 NEB_BOX = {3200,4754};
Vec2 GEO_BOX = {6370,731};

#define DIST_THRESHOLD      70
#define ANGLE_STRAIGHT      5
#define ANGLE_LATERAL_MAX   15
#define LATERAL_DIST_MAX    731
#define MAX_GRAB_ATTEMPTS   3

/* ══════════════════════════════════════════════════════════════
   INIT
   ══════════════════════════════════════════════════════════════ */

void robot_init(RobotCtx *ctx) {
    ctx->mission        = MISSION_TELEMETRY;
    ctx->prev_mission   = MISSION_TELEMETRY;
    ctx->action         = ACTION_IDLE;
    ctx->last_event     = EVT_NONE;

    ctx->x              = START_X;
    ctx->y              = START_Y;
    ctx->heading        = START_THETA;

    ctx->wp_index       = 0;
    ctx->nav_target.x   = START_X;
    ctx->nav_target.y   = START_Y;
    ctx->nav_angle      = 0;
    ctx->nav_dist       = 0;

    ctx->telemetry_pad  = 2;
    ctx->items_collected = 0;
    ctx->grab_attempts  = 0;
    ctx->neb_held       = 0;
    ctx->geo_held       = 0;

    ctx->start_tick     = get_global_tick();
    ctx->elapsed_ms     = 0;
}

/* ══════════════════════════════════════════════════════════════
   SENSOR UPDATE
   ══════════════════════════════════════════════════════════════ */

void sensor_update(RobotCtx *ctx) {
    ctx->heading = (i2c_rx_buffer[HEADING_MSB] << 8
                  | i2c_rx_buffer[HEADING_LSB]) / 16;

    /* TODO: update ctx->x, ctx->y from encoder dead reckoning */

    ctx->elapsed_ms = get_global_tick() - ctx->start_tick;
}

/* ══════════════════════════════════════════════════════════════
   NAVIGATION
   ══════════════════════════════════════════════════════════════ */

void nav_to(RobotCtx *ctx, uint16_t tx, uint16_t ty) {
    ctx->nav_target.x = tx;
    ctx->nav_target.y = ty;

    int32_t dx = (int32_t)tx - (int32_t)ctx->x;
    int32_t dy = (int32_t)ty - (int32_t)ctx->y;

    int32_t adx = dx < 0 ? -dx : dx;
    int32_t ady = dy < 0 ? -dy : dy;
    ctx->nav_dist = (int16_t)(adx + ady);

    /* TODO: ctx->nav_angle = norm_deg(atan2(dy, dx) * 180 / PI - ctx->heading); */
    ctx->nav_angle = 0;  // REPLACE THIS
}

int nav_drive(RobotCtx *ctx) {
    if (ctx->nav_dist < DIST_THRESHOLD) {
        ctx->action = ACTION_IDLE;
        return 1;
    }

    int16_t ang = ctx->nav_angle;
    int16_t abs_ang = ang < 0 ? -ang : ang;

    if (abs_ang < ANGLE_STRAIGHT) {
        ctx->action = ACTION_STEPPING;
        uint8_t step_dist = 20;
        int result = step((void*)&step_dist);
        if (!result) ctx->last_event = EVT_STEP_FAILED;

    } else if (abs_ang <= ANGLE_LATERAL_MAX && ctx->nav_dist <= LATERAL_DIST_MAX) {
        ctx->action = (ang > 0) ? ACTION_LATERAL_R : ACTION_LATERAL_L;
        uint8_t lat_dist = 10;
        if (ang > 0) lateral_right((void*)&lat_dist);
        else         lateral_left((void*)&lat_dist);

    } else {
        ctx->action = ACTION_ROTATING;
        int8_t rot_angle = (int8_t)ang;
        int result = rotate((void*)&rot_angle);
        if (!result) ctx->last_event = EVT_ROTATE_FAILED;
    }

    ctx->action = ACTION_IDLE;
    return 0;
}

/* ══════════════════════════════════════════════════════════════
   MISSION HANDLERS
   ══════════════════════════════════════════════════════════════ */

static Mission handle_telemetry(RobotCtx *ctx) {
    /* rotate to face west wall, read april tag for drop-off pad */
    int8_t rot = 90;
    rotate((void*)&rot);

    uint8_t tag = camera_check_apriltag();
    if (tag > 0 && tag <= 5) ctx->telemetry_pad = tag - 1;

    int8_t rot_back = -90;
    rotate((void*)&rot_back);

    return MISSION_GRAB_NEB;
}

static Mission handle_grab_neb(RobotCtx *ctx) {
    /* drive to nebulite container at NEB_BOX, pick it up */
    nav_to(ctx, NEB_BOX.x, NEB_BOX.y);
    while (!nav_drive(ctx)) {
        sensor_update(ctx);
        nav_to(ctx, NEB_BOX.x, NEB_BOX.y);
    }

    ctx->action = ACTION_GRABBING;
    /* TODO: activate intake/gripper */
    ctx->neb_held = 1;
    ctx->action = ACTION_IDLE;

    return MISSION_GRAB_GEO;
}

static Mission handle_grab_geo(RobotCtx *ctx) {
    /* drive to geodinium container at GEO_BOX, pick it up */
    nav_to(ctx, GEO_BOX.x, GEO_BOX.y);
    while (!nav_drive(ctx)) {
        sensor_update(ctx);
        nav_to(ctx, GEO_BOX.x, GEO_BOX.y);
    }

    ctx->action = ACTION_GRABBING;
    /* TODO: activate intake/gripper */
    ctx->geo_held = 1;
    ctx->action = ACTION_IDLE;

    return MISSION_SWEEP;
}

static Mission handle_sweep(RobotCtx *ctx) {
    /* follow waypoint array in lawnmower pattern
     * passive Intake→Processing→Sorting handles samples mechanically */
    if (ctx->wp_index >= NUM_WP) {
        ctx->wp_index = 0;
        return MISSION_DUMP_NEB;
    }

    Vec2 wp = WAYPOINTS[ctx->wp_index];
    nav_to(ctx, wp.x, wp.y);
    while (!nav_drive(ctx)) {
        sensor_update(ctx);
        nav_to(ctx, wp.x, wp.y);
        /* TODO: if (ctx->elapsed_ms > 170000) return MISSION_DUMP_NEB; */
    }

    ctx->wp_index++;
    return MISSION_SWEEP;
}

static Mission handle_dump_neb(RobotCtx *ctx) {
    /* navigate to rendezvous pad (learned from april tag), dump nebulite */
    Vec2 drop = RENDEZVOUS[ctx->telemetry_pad];
    nav_to(ctx, drop.x, drop.y);
    while (!nav_drive(ctx)) {
        sensor_update(ctx);
        nav_to(ctx, drop.x, drop.y);
    }

    ctx->action = ACTION_SCORING;
    /* TODO: activate dump mechanism */
    ctx->neb_held = 0;
    ctx->action = ACTION_IDLE;

    return MISSION_DUMP_GEO;
}

static Mission handle_dump_geo(RobotCtx *ctx) {
    /* navigate to rendezvous pad, dump geodinium */
    Vec2 drop = RENDEZVOUS[ctx->telemetry_pad];
    nav_to(ctx, drop.x, drop.y);
    while (!nav_drive(ctx)) {
        sensor_update(ctx);
        nav_to(ctx, drop.x, drop.y);
    }

    ctx->action = ACTION_SCORING;
    /* TODO: activate dump mechanism for geo */
    ctx->geo_held = 0;
    ctx->action = ACTION_IDLE;

    return MISSION_DONE;
}

static Mission handle_done(RobotCtx *ctx) {
    (void)ctx;
    turn_off_motors();
    return MISSION_DONE;
}

/* ── Dispatch table ──────────────────────────────────────────── */
typedef Mission (*MissionHandler)(RobotCtx *);

static const MissionHandler mission_handlers[MISSION_COUNT] = {
    [MISSION_TELEMETRY] = handle_telemetry,
    [MISSION_GRAB_NEB]  = handle_grab_neb,
    [MISSION_GRAB_GEO]  = handle_grab_geo,
    [MISSION_SWEEP]     = handle_sweep,
    [MISSION_DUMP_NEB]  = handle_dump_neb,
    [MISSION_DUMP_GEO]  = handle_dump_geo,
    [MISSION_DONE]      = handle_done,
};

/* ══════════════════════════════════════════════════════════════
   TICK
   ══════════════════════════════════════════════════════════════ */

void robot_tick(RobotCtx *ctx) {
    sensor_update(ctx);

    Mission next = mission_handlers[ctx->mission](ctx);

    if (next != ctx->mission) {
        ctx->prev_mission = ctx->mission;
        ctx->mission = next;
        ctx->action = ACTION_IDLE;
        ctx->grab_attempts = 0;
    }
}

/* ══════════════════════════════════════════════════════════════
   ROBOT_MAIN — OS task entry point

   Add to fn_table in task_queue.c:
       const func_t fn_table[] = {
           lcd_print,
           Sensor_Read_Wrapper,
           usart_begin,
           robot_main,
       };
   ══════════════════════════════════════════════════════════════ */

void robot_main(void *args) {
    (void)args;

    RobotCtx ctx;
    robot_init(&ctx);

    while (ctx.mission != MISSION_DONE) {
        robot_tick(&ctx);
    }

    turn_off_motors();
    while (1) { block(); }
}

/* ══════════════════════════════════════════════════════════════
   STUBS — replace with real hardware implementations
   ══════════════════════════════════════════════════════════════ */

uint8_t camera_check_apriltag(void) {
    /* TODO: talk to camera over I2C/UART, return tag ID or 0 */
    return 0;
}

int16_t get_heading(void) {
    return (i2c_rx_buffer[HEADING_MSB] << 8
          | i2c_rx_buffer[HEADING_LSB]) / 16;
}

/* ── Debug ───────────────────────────────────────────────────── */

const char* mission_name(Mission m) {
    switch (m) {
        case MISSION_TELEMETRY: return "TELEM";
        case MISSION_GRAB_NEB:  return "GRABN";
        case MISSION_GRAB_GEO:  return "GRABG";
        case MISSION_SWEEP:     return "SWEEP";
        case MISSION_DUMP_NEB:  return "DUMPN";
        case MISSION_DUMP_GEO:  return "DUMPG";
        case MISSION_DONE:      return "DONE ";
        default:                return "?????";
    }
}

const char* action_name(Action a) {
    switch (a) {
        case ACTION_IDLE:        return "IDLE ";
        case ACTION_ROTATING:    return "ROTAT";
        case ACTION_STEPPING:    return "STEP ";
        case ACTION_LATERAL_L:   return "LATL ";
        case ACTION_LATERAL_R:   return "LATR ";
        case ACTION_GRABBING:    return "GRAB ";
        case ACTION_SCORING:     return "SCORE";
        case ACTION_READING_TAG: return "RTAG ";
        default:                 return "?????";
    }
}
