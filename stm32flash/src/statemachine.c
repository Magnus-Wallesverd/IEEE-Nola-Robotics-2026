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

/* ── Camera UART protocol ────────────────────────────────────── */
#define CAM_FN_SEE_TAG  0x00    // 0=none, 1=front, 2=back, 3=both
#define CAM_FN_ID       0x01    // arg1: 1=front, 2=back
#define CAM_FN_DIST     0x02    // arg1: 1=front, 2=back
#define CAM_FN_ANG      0x03    // arg1: 1=front, 2=back
#define CAM_FN_YAW      0x04    // arg1: 1=front, 2=back
#define CAM_FRONT       0x01
#define CAM_BACK        0x02

/* Send [0xAA][fn][arg1][arg2][0x55], receive [0xAA][val][?][0x55].
 * Disables RX interrupt during transaction to avoid IRQ consuming bytes.
 * Returns the value byte (resp[1]), or 0xFF on timeout/bad frame. */
static uint8_t cam_request(uint8_t fn, uint8_t arg1, uint8_t arg2) {
    /* disable RX interrupt — we're polling directly */
    USART1->CR1 &= ~USART_RXNEIE;

    /* send 5-byte request */
    uint8_t pkt[5] = {0xAA, fn, arg1, arg2, 0x55};
    for (int i = 0; i < 5; i++) {
        while (!(USART1->ISR & USART_TXE));
        USART1->TDR = pkt[i];
    }
    while (!(USART1->ISR & USART_TC));
    USART1->ICR |= (1 << 6);    // clear TC flag

    /* receive 4-byte response */
    uint8_t resp[4] = {0};
    for (int i = 0; i < 4; i++) {
        uint32_t timeout = 200000;
        while (!(USART1->ISR & USART_RXNE) && --timeout);
        if (!timeout) {
            USART1->CR1 |= USART_RXNEIE;
            return 0xFF;    // timeout
        }
        resp[i] = (uint8_t)USART1->RDR;
    }

    USART1->CR1 |= USART_RXNEIE;

    if (resp[0] != 0xAA || resp[3] != 0x55) return 0xFF;
    return resp[1];
}

/* ── Integer math (no stdlib) ────────────────────────────────── */

/* Integer square root via Newton's method */
static uint16_t isqrt32(uint32_t n) {
    if (n == 0) return 0;
    uint32_t x = n;
    uint32_t y = (x + 1) >> 1;
    while (y < x) {
        x = y;
        y = (x + n / x) >> 1;
    }
    return (uint16_t)x;
}

/* Integer atan2 — returns bearing in degrees [-180, 180].
 * Coordinate convention: +x = east, +y = north (same as field).
 * Returns compass bearing: 0 = north, 90 = east, etc.
 * Max error ~9 degrees, sufficient for navigation. */
static int16_t atan2_bearing(int32_t dy, int32_t dx) {
    if (dx == 0 && dy == 0) return 0;

    /* scale down to prevent overflow while keeping ratio */
    while (dx > 1000 || dx < -1000 || dy > 1000 || dy < -1000) {
        dx >>= 1;
        dy >>= 1;
    }

    int32_t abs_dx = dx < 0 ? -dx : dx;
    int32_t abs_dy = dy < 0 ? -dy : dy;
    int32_t angle;

    /* atan approximation: atan(t) ≈ 45*t for |t| <= 1 */
    if (abs_dx >= abs_dy) {
        /* use atan(dy/dx), result near 0 or ±180 */
        angle = (45 * dy) / (dx == 0 ? 1 : dx);
        if (dx < 0) angle += (dy >= 0) ? 180 : -180;
    } else {
        /* use 90 - atan(dx/dy) */
        angle = 90 - (45 * dx) / (dy == 0 ? 1 : dy);
        if (dy < 0) angle -= 180;
    }

    /* convert from math convention (east=0) to compass (north=0) */
    angle = 90 - angle;
    if (angle > 180)  angle -= 360;
    if (angle <= -180) angle += 360;

    return (int16_t)angle;
}

/* Normalize heading error to [-180, 180] */
static int16_t heading_error(int16_t target, int16_t current) {
    int16_t err = target - current;
    if (err > 180)  err -= 360;
    if (err <= -180) err += 360;
    return err;
}

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

    /* true distance */
    ctx->nav_dist = (int16_t)isqrt32((uint32_t)(dx*dx + dy*dy));

    /* bearing to target (compass degrees, 0=north, 90=east) */
    int16_t bearing = atan2_bearing(dy, dx);

    /* angle error: how far we need to rotate from current heading */
    ctx->nav_angle = heading_error(bearing, ctx->heading);
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
    /* rotate to face west wall (tags 0-4 are all at x=0, mid-field) */
    int8_t rot = -90;   // -90 = turn left to face west
    rotate((void*)&rot);

    /* ask camera if it sees a tag */
    uint8_t sees = cam_request(CAM_FN_SEE_TAG, 0, 0);

    if (sees == 1 || sees == 3) {
        /* front camera sees a tag — get its ID */
        uint8_t tag_id = cam_request(CAM_FN_ID, CAM_FRONT, 0);
        if (tag_id <= 4) {
            ctx->telemetry_pad = tag_id; // tag 0-4 maps to RENDEZVOUS[0-4]
        }
    } else if (sees == 2) {
        /* only back camera sees it */
        uint8_t tag_id = cam_request(CAM_FN_ID, CAM_BACK, 0);
        if (tag_id <= 4) {
            ctx->telemetry_pad = tag_id;
        }
    }
    /* else: no tag seen, keep default pad 2 */

    /* rotate back to face north */
    int8_t rot_back = 90;
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
   CAMERA API — thin wrappers over cam_request()
   ══════════════════════════════════════════════════════════════ */

/* Returns 0=none, 1=front, 2=back, 3=both */
uint8_t camera_check_apriltag(void) {
    return cam_request(CAM_FN_SEE_TAG, 0, 0);
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
