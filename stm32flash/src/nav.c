#include "nav.h"
#include "robot_state.h"
#include "motors.h"
#include <stdint.h>

#define TOF_WALL_STOP   200     // mm, stop when wall this close
#define HDG_THRESH      5       // degrees close enough to heading

int nav_wall_ahead(void) {
    return g_robot.tof_fwd_mm <= TOF_WALL_STOP;
}

int nav_time_exceeded(uint32_t ms) {
    return g_robot.elapsed_ms >= ms;
}

int nav_drive_to_wall(void) {
    if (g_robot.tof_fwd_mm <= TOF_WALL_STOP) return 1;
    int8_t cm = 10;
    step(&cm);
    return 0;
}

int nav_step_cm(int8_t dist_cm) {
    return step(&dist_cm);
}

int nav_rotate_to(int16_t target_hdg) {
    int16_t ang = target_hdg - g_robot.heading;
    while (ang >  180) ang -= 360;
    while (ang < -180) ang += 360;
    if (ang > -HDG_THRESH && ang < HDG_THRESH) return 1;
    int8_t a = (int8_t)((ang > 127) ? 127 : (ang < -128) ? -128 : ang);
    return rotate(&a);
}

int nav_drive_past_x(int16_t x_thresh) {
    if (g_robot.x < x_thresh || g_robot.tof_fwd_mm <= TOF_WALL_STOP) return 1;
    int8_t cm = 10;
    step(&cm);
    return 0;
}
