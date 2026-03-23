#ifndef NAV_H
#define NAV_H

#include <stdint.h>

// returns 1 when complete, 0 while busy

int  nav_wall_ahead(void); // returns 1 if wall is within stopping distance
int  nav_time_exceeded(uint32_t ms); // returns 1 if time is almost up

int  nav_drive_to_wall(void); // drive fwd until wall detected
int  nav_step_cm(int8_t dist_cm); // take step fwd as specified
int  nav_rotate_to(int16_t target_hdg); // rotate to target heading
int  nav_drive_past_x(int16_t x_thresh); // drive until past an x threshold

#endif /* NAV_H */
