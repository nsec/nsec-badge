#include <string.h>

#include <app_timer.h>

#include "drivers/controls.h"
#include "drivers/display.h"

#include "application.h"
#include "gfx_effect.h"
#include "random.h"

#include "images/external/snake_pattern_collision_bitmap.h"
#include "images/external/snake_pattern_food_bitmap.h"
#include "images/external/snake_pattern_mirror_bitmap.h"
#include "images/external/snake_pattern_scale_bitmap.h"
#include "images/external/snake_pattern_steroids_bitmap.h"
#include "images/external/snake_pattern_trimmer_bitmap.h"
#include "images/external/snake_splash_10_bitmap.h"
#include "images/external/snake_splash_11_bitmap.h"
#include "images/external/snake_splash_12_bitmap.h"
#include "images/external/snake_splash_13_bitmap.h"
#include "images/external/snake_splash_14_bitmap.h"
#include "images/external/snake_splash_15_bitmap.h"
#include "images/external/snake_splash_1_bitmap.h"
#include "images/external/snake_splash_2_bitmap.h"
#include "images/external/snake_splash_3_bitmap.h"
#include "images/external/snake_splash_4_bitmap.h"
#include "images/external/snake_splash_5_bitmap.h"
#include "images/external/snake_splash_6_bitmap.h"
#include "images/external/snake_splash_7_bitmap.h"
#include "images/external/snake_splash_8_bitmap.h"
#include "images/external/snake_splash_9_bitmap.h"
#include "images/external/snake_splash_bitmap.h"

#define SNAKE_BUTTON_NONE 255

#define SNAKE_CONTENTS_EMPTY -1
#define SNAKE_CONTENTS_FOOD -2
#define SNAKE_CONTENTS_MIRROR -3
#define SNAKE_CONTENTS_STEROIDS -4
#define SNAKE_CONTENTS_TRIMMER -5

#define SNAKE_GOING_LEFT (p_state->position.dx == -1)
#define SNAKE_GOING_RIGHT (p_state->position.dx == 1)
#define SNAKE_GOING_UP (p_state->position.dy == -1)
#define SNAKE_GOING_DOWN (p_state->position.dy == 1)

#define SNAKE_GO_LEFT (p_state->position.dx = -1, p_state->position.dy = 0)
#define SNAKE_GO_RIGHT (p_state->position.dx = 1, p_state->position.dy = 0)
#define SNAKE_GO_UP (p_state->position.dx = 0, p_state->position.dy = -1)
#define SNAKE_GO_DOWN (p_state->position.dx = 0, p_state->position.dy = 1)

#define SNAKE_GRID_CELL(x, y) (y * SNAKE_GRID_WIDTH + x)
#define SNAKE_GRID_CELL_X(z) (z % SNAKE_GRID_WIDTH)
#define SNAKE_GRID_CELL_Y(z) (z / SNAKE_GRID_WIDTH)
#define SNAKE_GRID_HEIGHT 13
#define SNAKE_GRID_PX_BORDER 1
#define SNAKE_GRID_PX_CELL 5
#define SNAKE_GRID_PX_OFFSET_X 1
#define SNAKE_GRID_PX_OFFSET_Y 1
#define SNAKE_GRID_WIDTH 23

#define SNAKE_PATTERN_COLLISION 1
#define SNAKE_PATTERN_EMPTY 2
#define SNAKE_PATTERN_FOOD 3
#define SNAKE_PATTERN_MIRROR 4
#define SNAKE_PATTERN_SCALE 5
#define SNAKE_PATTERN_STEROIDS 6
#define SNAKE_PATTERN_TRIMMER 7

#define SNAKE_SIDEBAR_COLOR 0x2945

static uint8_t snake_sidebar_blob[1292];

APP_TIMER_DEF(snake_game_timer);

typedef struct SnakeGamePositionState {
    int8_t dx;
    int8_t dy;
    uint8_t head_x;
    uint8_t head_y;
    bool mirror;
    uint8_t prev_x;
    uint8_t prev_y;
    uint8_t shrink_delay;
    uint8_t tail_x;
    uint8_t tail_y;
    uint8_t trim_count;
} SnakeGamePositionState;

typedef struct SnakeGameState {
    SnakeGamePositionState position;

    bool collided;
    bool delay;
    uint8_t growth;
    uint8_t food_delay;
    int16_t grid[SNAKE_GRID_WIDTH * SNAKE_GRID_HEIGHT];

    uint32_t z1;
    uint32_t z2;
    uint32_t z3;
    uint32_t z4;
} SnakeGameState;

static const struct bitmap_ext *splash_animation[] = {
    &snake_splash_1_bitmap,  &snake_splash_2_bitmap,  &snake_splash_3_bitmap,
    &snake_splash_4_bitmap,  &snake_splash_5_bitmap,  &snake_splash_6_bitmap,
    &snake_splash_7_bitmap,  &snake_splash_8_bitmap,  &snake_splash_9_bitmap,
    &snake_splash_10_bitmap, &snake_splash_11_bitmap, &snake_splash_12_bitmap,
    &snake_splash_13_bitmap, &snake_splash_14_bitmap, &snake_splash_15_bitmap,
};

static uint8_t snake_button_read_value = SNAKE_BUTTON_NONE;

static void snake_buttons_handle(button_t button)
{
    switch (button) {
    case BUTTON_BACK:
    case BUTTON_DOWN:
    case BUTTON_ENTER:
    case BUTTON_UP:
        snake_button_read_value = button;
        break;

    default:
        break;
    }
}

static uint8_t snake_buttons_read()
{
    if (snake_button_read_value != SNAKE_BUTTON_NONE) {
        uint8_t value = snake_button_read_value;
        snake_button_read_value = SNAKE_BUTTON_NONE;

        return value;
    }

    return SNAKE_BUTTON_NONE;
}

#ifndef NSEC_FLAVOR_CONF
static void snake_init_sidebar_blob()
{
    memcpy(&snake_sidebar_blob,
           "\x4f\x45\x32\x6a\x72\xb2\xcb\x28\x3c\x7b\x21\x53\x58\x5c\x21\x53"
           "\x58\x40\x73\x49\x19\x5c\x64\x51\x4b\x5e\x78\x10\x01\x02\x21\x57"
           "\x50\x55\x72\x10\x56\x54\x21\x54\x58\x46\x60\x10\x50\x5c\x21\x5d"
           "\x40\x12\x69\x55\x58\x56\x4e\x44\x78\x65\x20\x42\x79\x73\x6b\x10"
           "\x7f\x66\x20\x44\x78\x65\x20\x53\x78\x69\x70\x10\x67\x61\x73\x10"
           "\x79\x67\x6e\x5f\x62\x65\x64\x10\x71\x6e\x64\x10\x75\x76\x65\x5e"
           "\x30\x61\x63\x53\x75\x70\x74\x55\x74\x04\x25\x9c\x44\x40\x24\x8c"
           "\x14\x08\x67\xd3\x14\x19\x61\xc8\x5b\x4d\x61\x90\x14\x5f\x61\xd3"
           "\x03\x04\x61\xdb\x5c\x08\x27\x8c\x1e\x45\x23\xd4\x14\x18\x24\x83"
           "\x13\x5c\x29\x9b\x51\x46\x66\xc8\x51\x03\x32\x9c\x45\x49\x32\x8a"
           "\x51\x49\x6d\xd8\x51\x03\x2f\xda\x5a\x46\x28\x8a\x14\x06\xb6\x70"
           "\xc9\x6a\x4d\xdb\x9d\x7c\x69\x1d\xfa\xfb\x85\xaf\xbb\xd5\x98\x0c"
           "\xbe\x7d\x5e\x80\x9f\x60\x62\x5e\xd2\x2b\x58\x14\xc9\xf8\x4f\x81"
           "\x46\x7c\xd0\x0c\xa6\x29\x49\xd8\x2e\x6c\x0a\x18\x4e\xbc\xc4\x45"
           "\x56\x0f\x0d\x5e\xb8\x6b\x74\xe2\xca\x6a\xe6\x8a\xb9\x2d\x05\x5c"
           "\xa6\x7e\x4d\x8d\xb4\x76\x8a\x63\xc5\xdb\x08\xcc\x33\x3b\x1a\x36"
           "\xf2\x1f\x0a\x11\xcb\xeb\xdf\x53\x7f\x96\x3b\x2d\xb1\x63\x5d\x4b"
           "\x48\x65\x10\x56\x1d\x60\xa4\xb0\xf9\x6a\x1d\x2c\xb7\x05\x2e\x98"
           "\x48\xfd\x69\x3e\x88\x32\xa6\x8d\x91\xf4\xad\xe7\xfe\xeb\xf1\x8c"
           "\xf4\x6a\x0a\x9f\x3f\xe6\xb5\x6e\xcc\x81\x84\x9d\xf2\xe5\x8b\x6a"
           "\x36\xdb\xee\xd8\xe2\x2d\xaa\x66\x18\xda\x08\x8a\xc8\xa3\x18\xed"
           "\x31\x86\x21\x1f\x51\xd6\x3a\xa3\xf2\xa7\x10\x1f\x9a\x06\x40\x38"
           "\x0f\x1a\x3e\xaf\x62\x12\xe4\x61\x39\xc6\xa3\xbd\x3b\x90\x2f\x61"
           "\x0a\xff\xe7\x37\xa0\x17\xd1\x52\x09\xf2\x5d\x20\x1a\x58\xe1\x2e"
           "\xd9\x52\xa3\xc3\x6a\x10\x23\x0c\xa5\x44\xf1\xe7\x4d\x0b\xec\xd5"
           "\xa1\x0b\x3e\x78\x66\x09\xe2\x15\x67\x9d\xda\x35\x48\x48\xa0\xcd"
           "\x22\x33\xe8\xff\x17\x5d\xc5\x50\xc8\x38\xb4\xd0\x11\x83\x2d\xb7"
           "\x4c\xc7\xcb\x2e\xae\x78\x94\x7b\x9e\x37\xcc\xaf\x94\x31\xcc\x5d"
           "\x09\x12\x55\x23\xa2\x42\x38\x3b\x3d\x20\x41\x20\x73\x65\x63\x72"
           "\x74\x74\x20\x67\x72\x6f\x75\x70\x20\x77\x69\x74\x68\x69\x6e\x20"
           "\x74\x68\x65\x20\x63\x6f\x6d\x6d\x75\xce\x69\x74\x79\x20\x69\x73"
           "\x20\x8c\x72\x79\x69\x6e\x67\x20\x74\x6f\x20\x6f\x76\x65\x72\x74"
           "\x68\x72\x6f\x77\x20\x74\x68\x65\x20\x63\x75\x72\x72\x65\x6e\x9d"
           "\x20\x67\x6f\x76\x65\x72\x6e\x6d\x65\x6e\x74\x45\x58\x6d\xa9\x01"
           "\xdf\xb9\x27\x40\xb8\x81\x7d\x40\x1f\x7e\xa9\x98\xa3\x6d\xc8\x43"
           "\x59\x7d\xf9\x40\x6f\xec\xe5\xa0\xce\x76\xa9\x41\xf6\x31\xe8\x89"
           "\xbc\x70\xfc\x1b\x35\x69\x2e\x4a\xd7\x16\xe7\x5b\x3b\x7c\x60\x03"
           "\xce\x6d\x87\xc1\xb3\x7d\xe5\x4a\x86\x3f\xee\xad\xbf\xe9\x33\xbe"
           "\x7a\x08\x85\x42\xb3\x1e\xb8\x7f\x16\x75\xcf\xab\xc9\xc5\xa6\x6f"
           "\xdf\x80\xb1\xbc\xc4\xc1\x01\x31\xc3\x59\x6f\x9d\x28\xfb\x7c\xc6"
           "\x71\x18\xb7\x06\xfd\x58\x3f\x22\x2e\xca\x6a\x1d\xec\x7e\x82\x93"
           "\x56\xfb\xd7\x07\xc1\xc9\xc5\x76\x67\x30\x6d\x99\xf4\xf7\xa2\x75"
           "\x59\xaf\x7f\x9a\xbe\xcc\xb8\x7d\x30\xfb\xbf\x99\x76\x1f\x9b\xe3"
           "\x79\x96\x5c\x24\x8e\xbc\x81\x9f\x48\xa6\x53\x40\xa1\x96\xbc\xfa"
           "\xbb\x03\xe9\x05\x96\xa6\xd8\x49\xd1\x4a\x98\x20\x9e\xfc\x8b\x27"
           "\xc0\x3d\xab\x84\x77\xe3\x04\xc3\xb7\xec\xae\x53\xd5\x54\xff\x03"
           "\xba\x13\x95\x46\xe5\x3d\xc8\x8f\x85\x31\xbb\x73\x8f\x6d\x69\xc5"
           "\x2e\x89\x75\x75\xa6\xb0\x6b\x73\xde\xc3\x57\xaa\xa2\x19\x09\x20"
           "\xca\x3f\xfe\xdb\xf7\xcf\xb0\xed\xee\xd3\x93\xe2\xc6\xb6\x36\x2b"
           "\x5a\x55\x31\xa7\x2b\xd7\x7c\x6c\x78\xff\x04\x35\xfc\x3f\x2e\xc9"
           "\x26\x23\xdf\x32\x0a\x18\x2f\x89\x48\x44\x11\x43\xe1\x9d\x47\x6e"
           "\x57\x7a\xb4\x73\x79\x88\x8a\xbf\x8b\x0d\x75\x23\xbd\x70\xc4\x66"
           "\x2a\x5e\xa7\xef\x4b\x3f\xcf\x4d\x0a\x79\xfa\x99\xd6\x9e\x06\x04"
           "\x5b\xea\x10\x8d\x81\x1b\x14\xf9\x3f\xa3\xf6\xa2\x55\x5f\x20\xb3"
           "\xa8\x03\x30\x8d\xb9\x4e\xe3\x50\xfd\x6f\xce\x1e\x4e\x93\x2c\xa5"
           "\x64\x92\xec\xa2\xf4\xa0\x75\xae\xd7\xd4\x27\xa1\x97\xd6\xe8\x61"
           "\x7e\xc5\x21\xa6\x6b\xba\xcc\xa8\xb1\x14\xea\xa2\x93\x71\x85\xe7"
           "\xda\x21\x77\xbb\x7f\xfe\x76\xd3\x01\xae\x3f\x33\x81\x7f\x73\x2f"
           "\x1e\xbf\x00\xce\x2d\xa9\x77\xa5\xcd\xfc\xc3\x8f\x7c\x54\x6e\xf9"
           "\x70\x04\x5e\x29\x04\xb1\x85\x09\xf2\xaf\x57\xe1\x09\x9e\x21\x92"
           "\x33\x8c\x4c\x3b\x3e\x8f\xba\xfd\xe5\x13\xe5\x0e\x38\x4f\xcf\x83"
           "\xd8\x2a\x18\x74\x07\xb0\x0a\x64\x04\x50\x0d\xd6\xe8\x86\x96\xa5"
           "\xf9\x19\xd2\x32\xce\x00\xc4\x3d\x22\xa6\xf7\xeb\xe8\x37\x9e\xe4"
           "\x2d\xc0\x7a\xad\x87\xce\x74\xcb\x1d\xd8\x5c\xb2\xca\x6c\xeb\x45"
           "\x9d\x23\x9a\x69\xe2\x81\xed\x91\x4b\xb4\xe4\x2c\x46\x91\x16\xd2"
           "\xa8\x1d\xdf\x01\x02\x07\x19\x87\x34\xc9\x24\xff\xce\x08\xa5\x04"
           "\xaf\x4a\x26\x13\xc5\x8b\x8a\x97\xc0\x5e\xb5\xf3\x4d\xc4\xea\xdb"
           "\x38\x41\x23\xab\xf1\xeb\xe1\x11\xb7\x19\x55\x1f\x0f\xb0\x2a\x3d"
           "\x43\xba\xa0\x45\x10\xcb\xe4\xa8\x78\x01\xe6\x90\xbd\xc9\x6e\x0f"
           "\xfe\x2c\xaa\x82\x9b\x4b\xfe\x1f\xd5\x12\xb7\x54\x2c\xe8\x88\xc7"
           "\x0a\x18\xb3\xb3\xd8\x9a\x88\x80\xb2\xf2\x77\xd8\x1c\x79\x92\x56"
           "\x0d\x51\xf3\x1c\x39\x11\xd0\xbe\xae\x6b\xef\x20\xc7\x26\x1d\xd2"
           "\x57\x5c\xc8\xf4\xfb\x27\x06\x6e\x17\x07\x06\x5f\x18\xca\x24\x1a"
           "\xc3\x8f\xc4\x5b\x96\x21\xff\x82\x69\x33\x0a\x1c\x16\x4d\x0a\x1f"
           "\xb7\x56\x42\x4c\x8f\x46\x95\x88\x32\x10\xe0\x70\x69\x3f\x13\x49"
           "\x0c\x21\xdb\xb7\xca\xa2\x17\x3b\xad\xa3\x1a\x5d\xdb\x8b\x67\xdb"
           "\x4f\x27\xc6\xb4\xe8\xf4\x56\x49\xc8\xaa\x2c\x82\x51\x52\xd3\x58"
           "\xe8\xc4\xce\xd7\xd4\xc7\xa8\xda\x47\x27\x8f\x9a\xf0\x61\x3f\x79"
           "\x2a\x4a\x50\x43\x97\x98\x95\xa0\x79\xb0\x7a\x27\x1f\xda\x27\xad"
           "\x88\x7f\xe9\xfc\xd1\xb0\x19\xc1\x35\xa4\x57\xad\xa8\xc3\xff\x88"
           "\x3c\x1a\xff\x3a\xa2\x77\xdc\x0a\x0a\xf4\x0c\x32",
           1292);
}
#else
static void snake_init_sidebar_blob()
{
    for (uint16_t i = 0; i < 1292; i++) {
        if (i % 2) {
            snake_sidebar_blob[i] = SNAKE_SIDEBAR_COLOR & 0xFF;
        } else {
            snake_sidebar_blob[i] = SNAKE_SIDEBAR_COLOR >> 8;
        }
    }
}
#endif

#ifndef NSEC_FLAVOR_CONF
static void snake_mutate_sidebar_blob(SnakeGameState *p_state)
{
    uint32_t b;
    b = ((p_state->z1 << 6) ^ p_state->z1) >> 13;
    p_state->z1 = ((p_state->z1 & 4294967294U) << 18) ^ b;
    b = ((p_state->z2 << 2) ^ p_state->z2) >> 27;
    p_state->z2 = ((p_state->z2 & 4294967288U) << 2) ^ b;
    b = ((p_state->z3 << 13) ^ p_state->z3) >> 21;
    p_state->z3 = ((p_state->z3 & 4294967280U) << 7) ^ b;
    b = ((p_state->z4 << 3) ^ p_state->z4) >> 12;
    p_state->z4 = ((p_state->z4 & 4294967168U) << 13) ^ b;

    uint32_t xor = p_state->z1 ^ p_state->z2 ^ p_state->z3 ^ p_state->z4;

    for (uint16_t i = 0; i < 1292; i += 4) {
        for (uint8_t j = 0; j < 4; j++) {
            snake_sidebar_blob[i + j] =
                snake_sidebar_blob[i + j] ^ ((xor >> (j * 8)) & 0xFF);
        }
    }

    for (uint16_t i = 0; i < 1292; i += p_state->growth) {
        snake_sidebar_blob[i] ^= ((xor >> 16) & 0xFF) ^ (p_state->growth - 1);
    }
}
#endif

static void snake_render_pattern(uint8_t x, uint8_t y, uint8_t pattern)
{
    uint8_t origin_x = x * (SNAKE_GRID_PX_CELL + SNAKE_GRID_PX_BORDER) +
                       SNAKE_GRID_PX_OFFSET_X + SNAKE_GRID_PX_BORDER;

    uint8_t origin_y = y * (SNAKE_GRID_PX_CELL + SNAKE_GRID_PX_BORDER) +
                       SNAKE_GRID_PX_OFFSET_Y + SNAKE_GRID_PX_BORDER;

    switch (pattern) {
    case SNAKE_PATTERN_EMPTY:
        gfx_fill_rect(origin_x, origin_y, SNAKE_GRID_PX_CELL,
                      SNAKE_GRID_PX_CELL, DISPLAY_BLACK);
        break;

    case SNAKE_PATTERN_COLLISION:
        display_draw_16bit_ext_bitmap(origin_x, origin_y,
                                      &snake_pattern_collision_bitmap, 0);
        break;

    case SNAKE_PATTERN_FOOD:
        display_draw_16bit_ext_bitmap(origin_x, origin_y,
                                      &snake_pattern_food_bitmap, 0);
        break;

    case SNAKE_PATTERN_MIRROR:
        display_draw_16bit_ext_bitmap(origin_x, origin_y,
                                      &snake_pattern_mirror_bitmap, 0);
        break;

    case SNAKE_PATTERN_SCALE:
        display_draw_16bit_ext_bitmap(origin_x, origin_y,
                                      &snake_pattern_scale_bitmap, 0);
        break;

    case SNAKE_PATTERN_STEROIDS:
        display_draw_16bit_ext_bitmap(origin_x, origin_y,
                                      &snake_pattern_steroids_bitmap, 0);
        break;

    case SNAKE_PATTERN_TRIMMER:
        display_draw_16bit_ext_bitmap(origin_x, origin_y,
                                      &snake_pattern_trimmer_bitmap, 0);
        break;
    }
}

static uint8_t snake_game_engine_detect_collision(SnakeGameState *p_state)
{
    SnakeGamePositionState *p_pos = &((*p_state).position);

    switch (p_state->grid[SNAKE_GRID_CELL(p_pos->head_x, p_pos->head_y)]) {
    case SNAKE_CONTENTS_EMPTY:
        return 0;

    case SNAKE_CONTENTS_FOOD:
        p_pos->shrink_delay++;
        return 1;

    case SNAKE_CONTENTS_MIRROR:
        p_pos->mirror = true;
        return 1;

    case SNAKE_CONTENTS_STEROIDS:
        p_pos->shrink_delay += 10;
        p_pos->trim_count = 0;
        return 1;

    case SNAKE_CONTENTS_TRIMMER:
        p_pos->shrink_delay = 0;
        p_pos->trim_count += 5;
        return 1;

    default:
        return 2;
    }
}

static void snake_game_engine_generate_food(SnakeGameState *p_state)
{
    int8_t contents;
    uint8_t pattern;

    if (p_state->food_delay > 0) {
        p_state->food_delay--;

        return;
    } else {
        p_state->food_delay = 30;
    }

    uint8_t chance = nsec_random_get_byte(100);
    if (chance >= 99) {
        contents = SNAKE_CONTENTS_MIRROR;
        pattern = SNAKE_PATTERN_MIRROR;
    } else if (chance >= 94) {
        contents = SNAKE_CONTENTS_TRIMMER;
        pattern = SNAKE_PATTERN_TRIMMER;
    } else if (chance >= 88) {
        contents = SNAKE_CONTENTS_STEROIDS;
        pattern = SNAKE_PATTERN_STEROIDS;
    } else {
        contents = SNAKE_CONTENTS_FOOD;
        pattern = SNAKE_PATTERN_FOOD;
    }

    uint8_t position =
        SNAKE_GRID_CELL(nsec_random_get_byte(SNAKE_GRID_WIDTH - 1),
                        nsec_random_get_byte(SNAKE_GRID_HEIGHT - 1));

    if (p_state->grid[position] == SNAKE_CONTENTS_EMPTY) {
        p_state->grid[position] = contents;

        snake_render_pattern(SNAKE_GRID_CELL_X(position),
                             SNAKE_GRID_CELL_Y(position), pattern);
    }
}

static void snake_game_engine_register_position(SnakeGameState *p_state)
{
    SnakeGamePositionState *p_pos = &((*p_state).position);

    p_state->grid[SNAKE_GRID_CELL(p_pos->prev_x, p_pos->prev_y)] =
        SNAKE_GRID_CELL(p_pos->head_x, p_pos->head_y);

    snake_render_pattern(p_pos->head_x, p_pos->head_y, SNAKE_PATTERN_SCALE);

    if (p_pos->shrink_delay > 0) {
        p_pos->shrink_delay--;
    } else {
        uint8_t trim = 1;
        if (p_pos->trim_count > 0) {
            p_pos->trim_count--;
            trim = 2;
        }

        for (uint8_t i = 0; i < trim; i++) {
            int16_t next_tail =
                p_state->grid[SNAKE_GRID_CELL(p_pos->tail_x, p_pos->tail_y)];

            p_state->grid[SNAKE_GRID_CELL(p_pos->tail_x, p_pos->tail_y)] =
                SNAKE_CONTENTS_EMPTY;

            snake_render_pattern(p_pos->tail_x, p_pos->tail_y,
                                 SNAKE_PATTERN_EMPTY);

            p_pos->tail_x = SNAKE_GRID_CELL_X(next_tail);
            p_pos->tail_y = SNAKE_GRID_CELL_Y(next_tail);
        }
    }
}

static void snake_game_engine_mirror_position(SnakeGameState *p_state)
{
    SnakeGamePositionState *p_pos = &((*p_state).position);

    if (p_pos->mirror) {
        p_pos->mirror = false;

        int16_t old_head = SNAKE_GRID_CELL(p_pos->head_x, p_pos->head_y);
        int16_t old_tail = SNAKE_GRID_CELL(p_pos->tail_x, p_pos->tail_y);

        // Fix movement direction after reversing the snake.
        int16_t next_tail = p_state->grid[old_tail];

        if (next_tail < old_tail) {
            if (old_tail - next_tail == 1) {
                p_pos->dx = 1;
                p_pos->dy = 0;
            } else {
                p_pos->dx = 0;
                p_pos->dy = 1;
            }
        } else {
            if (next_tail - old_tail == 1) {
                p_pos->dx = -1;
                p_pos->dy = 0;
            } else {
                p_pos->dx = 0;
                p_pos->dy = -1;
            }
        }

        int16_t new_head = old_tail;
        int16_t new_tail = old_tail;

        int16_t next = old_tail;
        int16_t prev = SNAKE_CONTENTS_EMPTY;

        while (new_tail >= 0) {
            next = p_state->grid[new_tail];
            p_state->grid[new_tail] = prev;
            prev = new_tail;
            new_tail = next;
        }

        new_tail = old_head;

        p_pos->head_x = SNAKE_GRID_CELL_X(new_head);
        p_pos->head_y = SNAKE_GRID_CELL_Y(new_head);

        p_pos->tail_x = SNAKE_GRID_CELL_X(new_tail);
        p_pos->tail_y = SNAKE_GRID_CELL_Y(new_tail);
    }
}

static void snake_game_engine_update_position(SnakeGameState *p_state)
{
    switch (snake_buttons_read()) {
    case BUTTON_BACK:
        if (SNAKE_GOING_DOWN || SNAKE_GOING_UP) {
            SNAKE_GO_LEFT;
        }
        break;

    case BUTTON_DOWN:
        if (SNAKE_GOING_LEFT || SNAKE_GOING_RIGHT) {
            SNAKE_GO_DOWN;
        }
        break;

    case BUTTON_ENTER:
        if (SNAKE_GOING_DOWN || SNAKE_GOING_UP) {
            SNAKE_GO_RIGHT;
        }
        break;

    case BUTTON_UP:
        if (SNAKE_GOING_LEFT || SNAKE_GOING_RIGHT) {
            SNAKE_GO_UP;
        }
        break;
    }

    p_state->position.prev_x = p_state->position.head_x;
    p_state->position.prev_y = p_state->position.head_y;

    p_state->position.head_x += p_state->position.dx;
    p_state->position.head_y += p_state->position.dy;

    if (p_state->position.head_x > SNAKE_GRID_WIDTH) {
        p_state->position.head_x = SNAKE_GRID_WIDTH - 1;
    } else if (p_state->position.head_x == SNAKE_GRID_WIDTH) {
        p_state->position.head_x = 0;
    }

    if (p_state->position.head_y > SNAKE_GRID_HEIGHT) {
        p_state->position.head_y = SNAKE_GRID_HEIGHT - 1;
    } else if (p_state->position.head_y == SNAKE_GRID_HEIGHT) {
        p_state->position.head_y = 0;
    }
}

static void snake_update_sidebar(SnakeGameState *p_state)
{
    char count[4];
    sprintf(count, "%03d", p_state->growth);

    gfx_set_text_background_color(DISPLAY_WHITE, SNAKE_SIDEBAR_COLOR);
    gfx_set_cursor(141, 2);
    gfx_puts(count);

    display_draw_16bit_bitmap(140, 45, (const uint8_t *)&snake_sidebar_blob, 19,
                              34, 0);

    gfx_update();
}

static void snake_boot_sequence(void (*service_device)())
{
    service_device();
    display_draw_16bit_ext_bitmap(0, 0, &snake_splash_bitmap, 0);

    snake_buttons_read();
    do {
        service_device();
    } while (snake_buttons_read() == SNAKE_BUTTON_NONE);

    for (uint8_t i = 0; i < 15; i++) {
        service_device();
        display_draw_16bit_ext_bitmap(40, 0, splash_animation[i], 0);
    }

    do {
        service_device();
    } while (snake_buttons_read() == SNAKE_BUTTON_NONE);
}

static void snake_initial_snake(SnakeGameState *p_state)
{
    uint8_t initial_length = 3;

    p_state->position = (SnakeGamePositionState){
        .dx = 1,
        .dy = 0,
        .head_x = 0,
        .head_y = 0,
        .mirror = false,
        .prev_x = 0,
        .prev_y = 0,
        .shrink_delay = 0,
        .tail_x = 0,
        .tail_y = 0,
        .trim_count = 0,
    };

    for (uint16_t i = 0; i < SNAKE_GRID_WIDTH * SNAKE_GRID_HEIGHT; i++) {
        p_state->grid[i] = SNAKE_CONTENTS_EMPTY;
    }

    SnakeGamePositionState *p_pos = &((*p_state).position);

    p_pos->head_x = p_pos->tail_x = (SNAKE_GRID_WIDTH / 2) - initial_length;
    p_pos->head_y = p_pos->tail_y = SNAKE_GRID_HEIGHT / 2;

    snake_render_pattern(p_pos->head_x, p_pos->head_y, SNAKE_PATTERN_SCALE);
    for (uint8_t i = 1; i < initial_length; i++) {
        p_pos->head_x++;

        p_state->grid[SNAKE_GRID_CELL(p_pos->head_x - 1, p_pos->head_y)] =
            SNAKE_GRID_CELL(p_pos->head_x, p_pos->head_y);

        snake_render_pattern(p_pos->head_x, p_pos->head_y, SNAKE_PATTERN_SCALE);
    }
}

static void snake_prepare_field(SnakeGameState *p_state)
{
    gfx_fill_rect(0, 0, DISPLAY_HEIGHT, DISPLAY_WIDTH, DISPLAY_BLACK);
    gfx_fill_rect(139, 0, 1, DISPLAY_HEIGHT, SNAKE_SIDEBAR_COLOR);
    gfx_fill_rect(140, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, SNAKE_SIDEBAR_COLOR);

    gfx_update();

    snake_update_sidebar(p_state);
}

static void snake_game_loop(SnakeGameState *p_state)
{
    if (p_state->collided) {
        if (snake_buttons_read() != SNAKE_BUTTON_NONE) {
            application_clear();
        }

        return;
    }

    if (p_state->delay) {
        return;
    } else {
        p_state->delay = true;
    }

    snake_game_engine_mirror_position(p_state);
    snake_game_engine_update_position(p_state);

    switch (snake_game_engine_detect_collision(p_state)) {
    case 2:
        snake_render_pattern(p_state->position.head_x, p_state->position.head_y,
                             SNAKE_PATTERN_COLLISION);

        p_state->collided = true;
        break;

    case 1:
        p_state->growth++;

#ifndef NSEC_FLAVOR_CONF
        snake_mutate_sidebar_blob(p_state);
#endif

        snake_update_sidebar(p_state);
        // fall through

    default:
        snake_game_engine_register_position(p_state);
        snake_game_engine_generate_food(p_state);
    }
}

static void snake_game_timer_callback(void *p_context)
{
    SnakeGameState *p_state = (SnakeGameState *)p_context;

    p_state->delay = false;
}

void snake_application(void (*service_device)())
{
    SnakeGameState state = {.collided = false,
                            .delay = true,
                            .food_delay = 0,
                            .z1 = 5,
                            .z2 = 14,
                            .z3 = 97,
                            .z4 = 372};

    nsec_controls_add_handler(snake_buttons_handle);

    snake_init_sidebar_blob();

    snake_boot_sequence(service_device);
    snake_prepare_field(&state);
    snake_initial_snake(&state);

    app_timer_create(&snake_game_timer, APP_TIMER_MODE_REPEATED,
                     snake_game_timer_callback);
    app_timer_start(snake_game_timer, APP_TIMER_TICKS(150), &state);

    while (application_get() == snake_application) {
        snake_game_loop(&state);
        service_device();
    }

    ret_code_t err_code = app_timer_stop(snake_game_timer);
    APP_ERROR_CHECK(err_code);

    nsec_controls_suspend_handler(snake_buttons_handle);
}
