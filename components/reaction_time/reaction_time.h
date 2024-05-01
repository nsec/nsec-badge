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

const button_t LVL1PAT[16] = {
    BUTTON_DOWN, BUTTON_UP,   BUTTON_RIGHT, BUTTON_LEFT,
    BUTTON_UP,   BUTTON_LEFT, BUTTON_RIGHT, BUTTON_DOWN,
    BUTTON_LEFT, BUTTON_DOWN, BUTTON_UP,    BUTTON_DOWN,
    BUTTON_UP,   BUTTON_LEFT, BUTTON_RIGHT, BUTTON_UP};
const button_t LVL2PAT[16] = {
    BUTTON_LEFT,  BUTTON_RIGHT, BUTTON_DOWN, BUTTON_UP,
    BUTTON_DOWN,  BUTTON_RIGHT, BUTTON_UP,   BUTTON_LEFT,
    BUTTON_RIGHT, BUTTON_DOWN,  BUTTON_LEFT, BUTTON_RIGHT,
    BUTTON_UP,    BUTTON_LEFT,  BUTTON_UP,   BUTTON_DOWN};

void register_reaction_time_cmd(void);

#ifdef __cplusplus
}
#endif
