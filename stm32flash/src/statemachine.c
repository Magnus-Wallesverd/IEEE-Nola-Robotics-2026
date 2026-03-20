#include <cstdint>
#include <stdint.h> 

typedef struct{ 
    uin8t_t WAITING; 
    uin8t_t NAVIGATING; 
    uint8_t MECHANICS;
}Active_State;

typedef struct{
    uin8t_t WAITING;
}Passive_State;

typedef struct{
    Global_State SLEEP;
    Global_State WAKEUP;
    Global_State GOALS;
    Global_State ROAMING;
    Global_State COMPLETE;
}Context;

typedef enum{
    EVT_APRILTAG = 0,
    EVT_ITEMFOUND,
    EVT_ITEMGRABBED,
    EVT_GRABFAILED
}Event;

typedef enum{
    TELEMETRY = 0,
    GRAB_NEB,
    GRAB_GEO,
    SWEEP,
    DUMP_NEB,
    DUMP_GEO,
    DONE
}Mission;

typedef enum{
    IDLE = 0,
    ROTATING,
    STEPPING
}Nav_State;

typedef struct { uint16_t x, y; } Vec2;

// all distance and coordinates are in jiawei's units
// just divide by 48 to get cm
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

#define NEB_BOX_X   3200
#define NEB_BOX_Y   4754
#define GEO_BOX_X   6370
#define GEO_BOX_Y   731

const Vec2 WAYPOINTS[] = {
    {1097,1097},
    {1097,4389},
    {5638,4389},
    {5638,1097},

    {1097,2194},
    {5638,2194},
    {5638,3353},
    {1097,3353},
    {1097,1097},
    {5638,1097},
 
    {5638,2743},
    {8625,2743},

    {8625,1097},
    {10241,1097},
    {10241,4389},
    {8625,4389},
    {8625,2743},

    {10241,2743},
    {10241,1097},
    {8625,1097},
    {8625,4389},
    {10241,4389},
    {10241,2743}
};

#define NUM_WP (sizeof(WAYPOINTS)/sizeof(WAYPOINTS[0]))

uint16_t robot_x = 3810;
uint16_t robot_y = 731;
int8_t robot_theta = 90; // facing north, degrees
int8_t telemetry_pad = 2;
Vec2 drop_target = RENDEZVOUS[2];

Mission mission = TELEMETRY;
uint32_t wp_index = 0;
Nav_State navState = IDLE;


Global_State ROBOT_STATE;
Global_State* ROBOT_STATE_P = &ROBOT_STATE;

// function to update these things
// gyroscope, encoders, cameras (apriltags)

// navigation
void nav_to(uint16_t tx, uint16_t ty, int8_t *ang, int16_t *dist){
    uint16_t dx = tx - robot_x;
    uint16_t dy = ty - robot_y;
    // get these calculation from the camera
    //*dist = sqrt(dx*dx + dy*dy);
    //*ang = norm_deg((atan2(dy,dx) - robot_theta));
}

void dispatch(int8_t ang, int16_t dist){
    if (dist < 70 || navState != IDLE) return;

    if (ang < )

    if (abs_a < 5) {
        // motor step
        navState = STEPPING;
    } else if (abs_a <= 15 && dist <= 731) {
        navState = STEPPING;
        // move laterally
    } else {
        // motor rotate
        navState = ROTATING;
    }
}

void missions(void){
    
    int8_t ang;
    int16_t dist;
    
    switch(mission) {
        case TELEMETRY:
            // find west wall tag, read drop off point
            break;
        case GRAB_NEB:
            // drive to nebulite container and pick it up
            break;
        case GRAB_GEO:
            // drive to geodinium container and pick it up
            break;
        case SWEEP:
            if (wp_index >= NUM_WP) {
                mission = DUMP_NEB;
                return;
            }

            nav_to(WAYPOINTS[wp_index].x, WAYPOINTS[wp_index].y, &ang, &dist);

            if (dist < 244) {
                wp_index++;
                return;
            }
            dispatch(ang,dist);
            break;
        case DUMP_NEB:
            // drop off nebulite
            break;
        case DUMP_GEO:
            // drop off geodinium
            break;
        case DONE:
            // end all processes
            break;
    }
}
// goal is to manage states Start in the WAIT State WAIT STATE FUNCTIONS

 /* 
 * * * Acess Camera * * *
 *    Find April Tag
 * * * * * * * * * * * * 
 *    IF NO APRIL TAG
 * CHANGE STATE TO NAVIGATE
 *    
 */

void main(void* args){

    // should wait for the start LED
    // should know when 3 mins are about to be up
    // update position from gyro
    // update position from encoders
    // parse camera stuff
    // sorting stuff
    // mission state machine

    if(ROBOT_STATE_P->WAITING){
      
    } else if(ROBOT_STATE_P->NAVIGATING){

    } else if(ROBOT_STATE_P->MECHANICS){
        
    } else {
        return;
    }
    
}
