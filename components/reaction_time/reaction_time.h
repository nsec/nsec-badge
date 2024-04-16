#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define NVS_NAMESPACE "storage"
#define NVS_LEVEL_KEY "rt_lvl"

typedef enum {
    BUTTON_UP = 0,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_COUNT, // Used to know number of buttons
} button_t;

void register_reaction_time_cmd(void);

#ifdef __cplusplus
}
#endif
