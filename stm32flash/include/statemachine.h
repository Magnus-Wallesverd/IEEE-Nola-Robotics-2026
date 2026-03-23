#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdint.h>

/* WAYPOINT */
typedef struct {
    uint16_t x, y;
    uint8_t  type;
    uint8_t  visited;
} Waypoint;

typedef enum {
    WP_OPEN = 0,
    WP_CAVE_ENTER,
    WP_CAVE,
    WP_CAVE_EXIT
} WpType;

/* MISSIONS */
typedef enum {
    MISSION_WAIT_START,
    MISSION_LAWN_OPEN,
    MISSION_WP_RECOVER,
    MISSION_CAVE_ENTER,
    MISSION_LAWN_CAVE,
    MISSION_EXIT_CAVE,
    MISSION_DONE,
    MISSION_COUNT
} Mission;

/* CONTEXT */
typedef struct {
    Mission  mission;
    Mission  prev_mission;

    int      sub_step;
    int      wp_target_idx;
    int      wp_nav_step;
    int      row_parity;    // 0=heading north, 1=south
    int      in_cave;

    uint32_t last_progress_tick;
} SM;

void sm_init(SM *s);
void sm_tick(SM *s);
void sm_main(void *args);

// Waypoint helpers
uint16_t wp_dist(Waypoint *w);
void     wp_check_visit(SM *s);
int      wp_nearest(SM *s, WpType type);
int      wp_all_done(WpType type);
int      wp_nav_to(SM *s);

#endif /* STATEMACHINE_H */
