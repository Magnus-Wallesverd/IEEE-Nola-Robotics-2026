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

typdef enum{
    EVT_APRILTAG
    EVT_ITEMFOUND
    EVT_ITEMGRABBED
    EVT_GRABFAILED
}Event;
Global_State ROBOT_STATE;
Global_State* ROBOT_STATE_P = &ROBOT_STATE;
// goal is to manage states Start in the WAIT State WAIT STATE FUNCTIONS
 * 
 * * * Acess Camera * * *
 *    Find April Tag
 * * * * * * * * * * * * 
 *    IF NO APRIL TAG
 * CHANGE STATE TO NAVIGATE
 *    
 */

void main(void* args){

    if(ROBOT_STATE_P->WAITING){
      
    } else if(ROBOT_STATE_P->NAVIGATING){

    } else if(ROBOT_STATE_P->MECHANICS){
        
    } else {
        return;
    }
    
}
