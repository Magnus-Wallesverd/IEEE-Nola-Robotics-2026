#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdint.h>

/* ══════════════════════════════════════════════════════════════
   TWO-LEVEL STATE MACHINE

   Level 1: Mission — WHAT the robot is trying to accomplish
     TELEMETRY → GRAB_NEB → GRAB_GEO → SWEEP → DUMP_NEB → DUMP_GEO → DONE

   Level 2: Action — HOW it's doing the current step right now
     IDLE → ROTATING → STEPPING → IDLE (navigation cycle)
     GRABBING, SCORING, DUMPING (mechanical actions)

   The mission level decides the destination/goal.
   The action level drives motors to get there.
   ══════════════════════════════════════════════════════════════ */

/* ── Mission: the high-level goal sequence ──────────────────── */
typedef enum {
    MISSION_TELEMETRY = 0,  // read april tag → learn drop-off pad
    MISSION_GRAB_NEB,       // drive to nebulite container, pick up
    MISSION_GRAB_GEO,       // drive to geodinium container, pick up
    MISSION_SWEEP,          // lawnmower the field collecting samples
    MISSION_DUMP_NEB,       // go to NEB_BOX, deposit
    MISSION_DUMP_GEO,       // go to GEO_BOX, deposit
    MISSION_DONE,           // stop everything
    MISSION_COUNT
} Mission;

/* ── Action: what the robot is physically doing right now ───── */
typedef enum {
    ACTION_IDLE = 0,        // waiting / deciding
    ACTION_ROTATING,        // turning to face target
    ACTION_STEPPING,        // driving toward target
    ACTION_LATERAL_L,       // strafing left
    ACTION_LATERAL_R,       // strafing right
    ACTION_GRABBING,        // closing gripper / intake
    ACTION_SCORING,         // depositing item
    ACTION_READING_TAG,     // camera looking for april tag
    ACTION_COUNT
} Action;

/* ── Events from sensors ────────────────────────────────────── */
typedef enum {
    EVT_NONE = 0,
    EVT_APRILTAG_SEEN,      // camera found april tag
    EVT_ARRIVED,            // close enough to waypoint
    EVT_ROTATE_DONE,        // heading aligned
    EVT_STEP_DONE,          // step() returned success
    EVT_STEP_FAILED,        // step() timed out
    EVT_ROTATE_FAILED,      // rotate() timed out
    EVT_GRAB_DONE,          // item grabbed
    EVT_GRAB_FAILED,        // grab missed
    EVT_DUMP_DONE,          // item deposited
    EVT_TIMEOUT,            // 3 min clock approaching
} Event;

/* ── Vec2: position in jiawei's units (÷48 = cm) ───────────── */
typedef struct {
    uint16_t x;
    uint16_t y;
} Vec2;

/* ── Robot context: all state lives here ────────────────────── */
typedef struct {
    /* state machine */
    Mission     mission;
    Mission     prev_mission;
    Action      action;
    Event       last_event;

    /* position tracking */
    uint16_t    x;
    uint16_t    y;
    int16_t     heading;        // degrees, from BNO055

    /* navigation */
    uint32_t    wp_index;       // current waypoint in sweep
    Vec2        nav_target;     // where we're trying to go
    int16_t     nav_angle;      // angle to target
    int16_t     nav_dist;       // distance to target

    /* mission data */
    uint8_t     telemetry_pad;  // drop-off pad from april tag (0-4)
    uint8_t     items_collected;
    uint8_t     grab_attempts;
    uint8_t     neb_held;       // 1 if carrying nebulite
    uint8_t     geo_held;       // 1 if carrying geodinium

    /* timing */
    uint32_t    start_tick;
    uint32_t    elapsed_ms;
} RobotCtx;

/* ── Waypoint / field data ──────────────────────────────────── */
extern const Vec2 TAG_WORLD[8];
extern const Vec2 RENDEZVOUS[5];
extern const Vec2 WAYPOINTS[];
extern Vec2 NEB_BOX;
extern Vec2 GEO_BOX;

/* ── Public API ─────────────────────────────────────────────── */
void robot_init(RobotCtx *ctx);
void robot_tick(RobotCtx *ctx);
void robot_main(void *args);

/* ── Navigation helpers ─────────────────────────────────────── */
void nav_to(RobotCtx *ctx, uint16_t tx, uint16_t ty);
int  nav_drive(RobotCtx *ctx);

/* ── Sensor interface (YOU implement these) ─────────────────── */
void    sensor_update(RobotCtx *ctx);
uint8_t camera_check_apriltag(void);
int16_t get_heading(void);

/* ── Debug ──────────────────────────────────────────────────── */
const char* mission_name(Mission m);
const char* action_name(Action a);

#endif // STATEMACHINE_H
