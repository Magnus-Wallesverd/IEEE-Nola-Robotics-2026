#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdint.h>

typedef struct{ 
    uin8t_t WAITING; 
    uin8t_t NAVIGATING; 
    uint8_t MECHANICS;
} Active_State;

typedef struct{
    uin8t_t WAITING;
} Passive_State;

typedef struct{
    Global_State SLEEP;
    Global_State WAKEUP;
    Global_State GOALS;
    Global_State ROAMING;
    Global_State COMPLETE;
} Context;

typedef enum{
    EVT_APRILTAG = 0,
    EVT_ITEMFOUND,
    EVT_ITEMGRABBED,
    EVT_GRABFAILED
} Event;

typedef enum{
    TELEMETRY = 0,
    GRAB_NEB,
    GRAB_GEO,
    SWEEP,
    DUMP_NEB,
    DUMP_GEO,
    DONE
} Mission;

typedef enum{
    IDLE = 0,
    ROTATING,
    STEPPING
} Nav_State;

typedef struct { 
    uint16_t x; 
    uint16_t y; 
} Vec2;

void nav_to(uint16_t tx, uint16_t ty, int8_t *ang, int16_t *dist);
void dispatch(int8_t ang, int16_t dist);
void missions(void);

#endif // STATEMACHINE_H
