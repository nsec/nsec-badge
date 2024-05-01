#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define CMD_NAME "reaction_time"
#define LOG_TAG CMD_NAME
#define NVS_NAMESPACE "storage"
#define NVS_LEVEL_KEY "rt_lvl"

typedef enum {
    BUTTON_UP = 35,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
} button_t;

const int32_t BUTTONS[] = {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
};

const int BUTTON_COUNT = sizeof(BUTTONS) / sizeof(*BUTTONS);

void register_reaction_time_cmd(void);

#ifdef __cplusplus
}
#endif
