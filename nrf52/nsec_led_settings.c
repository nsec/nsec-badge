/*
 * Copyright 2018 Eric Tremblay <habscup@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <nrf.h>
#include <nordic_common.h>
#include <stdio.h>
#include <stdbool.h>
#include "ble/nsec_ble.h"
#include "status_bar.h"
#include "app_glue.h"
#include "controls.h"
#include "nsec_led_settings.h"
#include "nsec_settings.h"
#include "menu.h"
#include "ssd1306.h"
#include "ws2812fx.h"

#define BRIGHNESS_MENU_INDEX        0


#define SUPER_LOW_BRIGHTNESS_INDEX  0
#define LOW_BRIGHTNESS_INDEX        1
#define MEDIUM_BRIGHTNESS_INDEX     2
#define HIGH_BRIGHTNESS_INDEX       3
#define MAX_BRIGHTNESS_INDEX        4

#define SUPER_SLOW_SPEED_INDEX      0
#define SLOW_SPEED_INDEX            1
#define MEDIUM_SPEED_INDEX          2
#define FAST_SPEED_INDEX            3
#define SUPER_FAST_SPEED_INDEX      4

#define RED_INDEX                   0
#define GREEN_INDEX                 1
#define BLUE_INDEX                  2
#define WHITE_INDEX                 3
#define BLACK_INDEX                 4
#define YELLOW_INDEX                5
#define CYAN_INDEX                  6
#define MAGENTA_INDEX               7
#define PURPLE_INDEX                8
#define ORANGE_INDEX                9

#define SUPER_LOW_BRIGHTNESS        20
#define LOW_BRIGHTNESS              40
#define MEDIUM_BRIGHTNESS           60
#define HIGH_BRIGHTNESS             80
#define MAX_BRIGHTNESS              100

#define SUPER_SLOW_SPEED            5000
#define SLOW_SPEED                  1000
#define MEDIUM_SPEED                500
#define FAST_SPEED                  100
#define SUPER_FAST_SPEED            10

enum setting_state {
    SETTING_STATE_CLOSED,
    SETTING_STATE_MENU,
    SETTING_STATE_BRIGHTNESS,
    SETTING_STATE_SPEED,
    SETTING_STATE_FAVORITE_COLOR,
    SETTING_STATE_DEFAULT_PATTERN,
};

static enum setting_state _state = SETTING_STATE_CLOSED;

static void show_brightness_menu(uint8_t item);
static void show_speed_menu(uint8_t item);
static void show_color_menu(uint8_t item);
static void save_brightness(uint8_t item);
static void save_speed(uint8_t item);
static void save_color(uint8_t item);

static void setting_handle_buttons(button_t button);

static menu_item_s settings_items[] = {
    {
        .label = "Led brightness",
        .handler = show_brightness_menu,
    }, {
        .label = "Led speed",
        .handler = show_speed_menu,
    }, {
        .label = "Favorite color",
        .handler = show_color_menu,
    }, {
        .label = "Default led pattern",
        .handler = NULL,
    }, {
        .label = "Restore factory default",
        .handler = NULL,
    }
};

static menu_item_s brightness_items[] = {
    {
        .label = "Super low",
        .handler = save_brightness,
    }, {
        .label = "Low",
        .handler = save_brightness,
    }, {
        .label = "Medium",
        .handler = save_brightness,
    }, {
        .label = "High",
        .handler = save_brightness,
    }, {
        .label = "Max",
        .handler = save_brightness,
    }
};

static menu_item_s speed_items[] = {
    {
        .label = "Super slow",
        .handler = save_speed,
    }, {
        .label = "Slow",
        .handler = save_speed,
    }, {
        .label = "Medium",
        .handler = save_speed,
    }, {
        .label = "Fast",
        .handler = save_speed,
    }, {
        .label = "Super fast",
        .handler = save_speed,
    }
};

static menu_item_s color_items[] = {
    {
        .label = "Red",
        .handler = save_color,
    }, {
        .label = "Green",
        .handler = save_color,
    }, {
        .label = "Blue",
        .handler = save_color,
    }, {
        .label = "White",
        .handler = save_color,
    }, {
        .label = "Black",
        .handler = save_color,
    }, {
        .label = "Yellow",
        .handler = save_color,
    }, {
        .label = "Cyan",
        .handler = save_color,
    }, {
        .label = "Magenta",
        .handler = save_color,
    }, {
        .label = "Purple",
        .handler = save_color,
    }, {
        .label = "Orange",
        .handler = save_color,
    } 
};

void nsec_show_led_settings(void) {
    gfx_fillRect(0, 8, 128, 65, SSD1306_BLACK);
    menu_init(0, 12, 128, 64 - 12, ARRAY_SIZE(settings_items), settings_items);
    nsec_controls_add_handler(setting_handle_buttons);
    _state = SETTING_STATE_MENU;
}

void show_actual_brightness(void) {
    uint8_t brightness = getBrightness_WS2812FX();
    char actual[50] = {0};
    if (brightness <= SUPER_LOW_BRIGHTNESS) {
        snprintf(actual, 50, "Now: %s", "Super Low");
    } else  if (brightness <= LOW_BRIGHTNESS) {
        snprintf(actual, 50, "Now: %s", "Low");
    } else if (brightness <= MEDIUM_BRIGHTNESS) {
        snprintf(actual, 50, "Now: %s", "Medium");
    } else if (brightness <= HIGH_BRIGHTNESS) {
        snprintf(actual, 50, "Now: %s", "High");
    } else {
        snprintf(actual, 50, "Now: %s", "Max");
    }

    gfx_fillRect(12, 20, 128, 65, SSD1306_BLACK);
    gfx_setCursor(0, 12);
    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(actual);
}

static void show_brightness_menu(uint8_t item) {
    menu_close();
    gfx_fillRect(0, 8, 128, 65, SSD1306_BLACK);
    show_actual_brightness();
    menu_init(0, 24, 128, 64 - 24, ARRAY_SIZE(brightness_items), brightness_items);
    _state = SETTING_STATE_BRIGHTNESS;
}

static void save_brightness(uint8_t item) {
    switch(item) {
        case SUPER_LOW_BRIGHTNESS_INDEX:
            setBrightness_WS2812FX(SUPER_LOW_BRIGHTNESS);
            break;
        case LOW_BRIGHTNESS_INDEX:
            setBrightness_WS2812FX(LOW_BRIGHTNESS);
            break;
        case MEDIUM_BRIGHTNESS_INDEX:
            setBrightness_WS2812FX(MEDIUM_BRIGHTNESS);
            break;
        case HIGH_BRIGHTNESS_INDEX:
            setBrightness_WS2812FX(HIGH_BRIGHTNESS);
            break;
        case MAX_BRIGHTNESS_INDEX:
            setBrightness_WS2812FX(MAX_BRIGHTNESS);
            break;
        default:
            break;
    }

    //TODO save into default_led_config whatever
    show_brightness_menu(0);
}

void show_actual_speed(void) {
    uint16_t speed = getSpeed_WS2812FX();
    char actual[50] = {0};
    if (speed >= SUPER_SLOW_SPEED) {
        snprintf(actual, 50, "Now: %s", "Super slow");
    } else  if (speed >= SLOW_SPEED) {
        snprintf(actual, 50, "Now: %s", "Slow");
    } else if (speed >= MEDIUM_SPEED) {
        snprintf(actual, 50, "Now: %s", "Medium");
    } else if (speed >= FAST_SPEED) {
        snprintf(actual, 50, "Now: %s", "Fast");
    } else {
        snprintf(actual, 50, "Now: %s", "Super fast");
    }

    gfx_fillRect(12, 20, 128, 65, SSD1306_BLACK);
    gfx_setCursor(0, 12);
    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(actual);
}

static void show_speed_menu(uint8_t item) {
    menu_close();
    gfx_fillRect(0, 8, 128, 65, SSD1306_BLACK);
    show_actual_speed();
    menu_init(0, 24, 128, 64 - 24, ARRAY_SIZE(speed_items), speed_items);
    _state = SETTING_STATE_SPEED;
}

static void save_speed(uint8_t item) {
    switch(item) {
        case SUPER_SLOW_SPEED_INDEX:
            setSpeed_WS2812FX(SUPER_SLOW_SPEED);
            break;
        case SLOW_SPEED_INDEX:
            setSpeed_WS2812FX(SLOW_SPEED);
            break;
        case MEDIUM_SPEED_INDEX:
            setSpeed_WS2812FX(MEDIUM_SPEED);
            break;
        case FAST_SPEED_INDEX:
            setSpeed_WS2812FX(FAST_SPEED);
            break;
        case SUPER_FAST_SPEED_INDEX:
            setSpeed_WS2812FX(SUPER_FAST_SPEED);
            break;
        default:
            break;
    }

    //TODO save into default_led_config whatever
    show_speed_menu(0);
}

void show_actual_color(void) {
    uint32_t color = getColor_WS2812FX();
    char actual[50] = {0};
    if (color == RED) {
        snprintf(actual, 50, "Now: %s", "Red");
    } else  if (color == GREEN) {
        snprintf(actual, 50, "Now: %s", "Green");
    } else if (color == BLUE) {
        snprintf(actual, 50, "Now: %s", "Blue");
    } else if (color == WHITE) {
        snprintf(actual, 50, "Now: %s", "White");
    } else if (color == BLACK) {
        snprintf(actual, 50, "Now: %s", "Black");
    } else if (color == YELLOW) {
        snprintf(actual, 50, "Now: %s", "Yellow");
    } else if (color == CYAN) {
        snprintf(actual, 50, "Now: %s", "Cyan");
    } else if (color == PURPLE) {
        snprintf(actual, 50, "Now: %s", "Purple");
    } else {
        snprintf(actual, 50, "Now: %s", "Orange");
    }

    gfx_fillRect(12, 20, 128, 65, SSD1306_BLACK);
    gfx_setCursor(0, 12);
    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(actual);
}

static void show_color_menu(uint8_t item) {
    menu_close();
    gfx_fillRect(0, 8, 128, 65, SSD1306_BLACK);
    show_actual_color();
    menu_init(0, 24, 128, 64 - 24, ARRAY_SIZE(color_items), color_items);
    _state = SETTING_STATE_FAVORITE_COLOR;
}

static void save_color(uint8_t item) {
    switch(item) {
        case RED_INDEX:
            setColor_packed_WS2812FX(RED);
            break;
        case GREEN_INDEX:
            setColor_packed_WS2812FX(GREEN);
            break;
        case BLUE_INDEX:
            setColor_packed_WS2812FX(BLUE);
            break;
        case WHITE_INDEX:
            setColor_packed_WS2812FX(WHITE);
            break;
        case BLACK_INDEX:
            setColor_packed_WS2812FX(BLACK);
            break;
        case YELLOW_INDEX:
            setColor_packed_WS2812FX(YELLOW);
            break;
        case CYAN_INDEX:
            setColor_packed_WS2812FX(CYAN);
            break;
        case MAGENTA_INDEX:
            setColor_packed_WS2812FX(MAGENTA);
            break;
        case PURPLE_INDEX:
            setColor_packed_WS2812FX(PURPLE);
            break;
        case ORANGE_INDEX:
            setColor_packed_WS2812FX(ORANGE);
            break;
        default:
            break;
    }

    //TODO save into default_led_config whatever
    show_color_menu(0);
}

static void setting_handle_buttons(button_t button) {
    if (button == BUTTON_BACK) {
        switch (_state) {
            case SETTING_STATE_MENU:
                _state = SETTING_STATE_CLOSED;
                menu_close();
                nsec_setting_show();
                break;

            case SETTING_STATE_BRIGHTNESS:
            case SETTING_STATE_SPEED:
            case SETTING_STATE_FAVORITE_COLOR:
                _state = SETTING_STATE_MENU;
                menu_close();
                nsec_show_led_settings();
                break;

            default:
                break;
        }
    }
}
