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

static const char *ERROR_MESSAGES[] = {
    "Oops! Looks like your reflexes need a firmware update. Try again!\n",
    "Too slow! Hit that button like you mean it next time.\n",
    "Missed it by a mile! Sharpen those reflexes and give it another go!\n",
    "Alert! Fast reaction required. This isn't a drill. Retry!\n",
    "Whoops! You blinked and missed it. Blink faster, or just try again!\n",
};

const int ERROR_MESSAGE_COUNT =
    sizeof(ERROR_MESSAGES) / sizeof(*ERROR_MESSAGES);

void register_reaction_time_cmd(void);

#ifdef __cplusplus
}
#endif
