#include "statemachine.h"

// all distance and coordinates are in jiawei's units
// just divide by 48 to get cm

const Vec2 TAG_WORLD[8] = { // where the tags are located
    [0] = {0,2743},
    [1] = {0,2743},
    [2] = {0,2743},
    [3] = {0,2743},
    [4] = {0,2743},
    [5] = {3885,5486},
    [6] = {5313,0},
    [7] = {11338,2743}
};

const Vec2 RENDEZVOUS[5] = { // where the drop off points are located
    [0] = {731,518},
    [1] = {731,1646},
    [2] = {731,2743},
    [3] = {731,3840},
    [4] = {731,4968}
};

const Vec2 WAYPOINTS[] = { // will probs need to change
    {1097,1097}, // perimeter outside cave
    {1097,4389},
    {5638,4389},
    {5638,1097},

    {1097,2194}, // lawnmower outside cave
    {5638,2194},
    {5638,3353},
    {1097,3353},
    {1097,1097},
    {5638,1097},
 
    {5638,2743}, // going into cave
    {8625,2743},

    {8625,1097}, // perimeter in cave
    {10241,1097},
    {10241,4389},
    {8625,4389},
    {8625,2743},

    {10241,2743}, // lawnmower in cave
    {10241,1097},
    {8625,1097},
    {8625,4389},
    {10241,4389},
    {10241,2743}
};

#define NUM_WP (sizeof(WAYPOINTS)/sizeof(WAYPOINTS[0]))

#define START_X     3810
#define START_Y     731
#define START_THETA 90  // facing north, degrees

Vec2 NEB_BOX = {3200,4754};
Vec2 GEO_BOX = {6370,731};

uint16_t robot_x = START_X;
uint16_t robot_y = START_Y;
int8_t robot_theta = START_THETA;   // facing north, degrees

int8_t telemetry_pad = 2;           // default pad is in the middle
Vec2 drop_target = RENDEZVOUS[2]; 

Mission mission = TELEMETRY;        // starting mission
uint32_t wp_index = 0;
Nav_State navState = IDLE;

Global_State ROBOT_STATE;
Global_State* ROBOT_STATE_P = &ROBOT_STATE;

// functions to update these things
// gyroscope, encoders, cameras (apriltags), ToF

// navigation
void nav_to(uint16_t tx, uint16_t ty, int8_t *ang, int16_t *dist){ // calculates distance and angle
    uint16_t dx = tx - robot_x;
    uint16_t dy = ty - robot_y;
    // get sqrt/atan function from the camera
    //*dist = sqrt(dx*dx + dy*dy);
    //*ang = norm_deg((atan2(dy,dx) - robot_theta));
}

void dispatch(int8_t ang, int16_t dist){
    if (dist < 70 || navState != IDLE) return;

    if (ang < 0) {
        ang = -ang;
    }

    if (ang < 5) {
        // motor step
        navState = STEPPING;
    } else if (ang <= 15 && dist <= 731) {
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
            // could probably just rotate -45 degrees to read it
            // mission = GRAB_NEB;
            break;
        case GRAB_NEB:
            // drive to nebulite container and pick it up
            mission = GRAB_GEO;
            break;
        case GRAB_GEO:
            // drive to geodinium container and pick it up
            // mission = SWEEP;
            break;
        case SWEEP:
            if (wp_index >= NUM_WP) { // after pathing complete
                mission = DUMP_NEB;
                return;
            }

            nav_to(WAYPOINTS[wp_index].x, WAYPOINTS[wp_index].y, &ang, &dist);

            if (dist < 100) { // if distance is small enough to reach wp
                wp_index++;
                return;
            }

            dispatch(ang,dist);
            break;
        case DUMP_NEB:
            // drop off nebulite
            mission = DUMP_GEO;
            break;
        case DUMP_GEO:
            // drop off geodinium
            mission = DONE;
            break;
        case DONE:
            // end all processes
            break;
    }
}

// goal is to manage states, Start in the WAIT State, WAIT STATE FUNCTIONS

 /* 
 * * * Acess Camera * * *
 *    Find April Tag
 * * * * * * * * * * * * 
 *    IF NO APRIL TAG
 * CHANGE STATE TO NAVIGATE
 *    
 */

// should wait for the start LED
// should know when 3 mins are about to be up
// update stuff from gyro
// update stuff from encoders
// update stuff from camera
// update stuff from ToF
// sorting stuff, passive
// mission state machine

// void main(void* args){
//
//     if(ROBOT_STATE_P->WAITING){
//
//     } else if(ROBOT_STATE_P->NAVIGATING){
//
//     } else if(ROBOT_STATE_P->MECHANICS){
//
//     } else {
//         return;
//     }
//
// }
