#include "menu.h"

#include "buttons.h"
#include "cmd_wifi.h"
#include "esp_err.h"
#include "esp_log.h"
#include "graphics.h"
#include "neopixel.h"
#include "save.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <cstring>

// Custom DSL for building menu from a set of standard widgets.

/**
 * Change the horizontal position of the widget by a few pixels.
 *
 * This is needed for the TEXT widget that must be rendered a little bit to the
 * left, compared to the other widgets, for correct visual alignment.
 */
#define INSET                                                                  \
    for (int __i = 1, __widget_x_save = __widget_x; __i > 0; --__i)            \
        for (int __j = 1, __widget_x = __widget_x_save - 5; __j > 0; --__j)

/**
 * Start the menu rendering by displaying the correct backround image.
 */
#define MENU(base)                                                             \
    do {                                                                       \
        graphics_draw_from_library(LIBRARY_IMAGE_MENU_B_##base, 0, 0);         \
    } while (0)

/**
 * Increment the horizontal position for the next widget.
 *
 * Must be used inside of a loop while rendering a row of segmented widgets.
 */
#define NEXT_SEGMENT (__widget_x += 15)

/**
 * Change the vertial position of the widget by a single pixel.
 *
 * This may be needed in some configuration to achieve the correct visual
 * alignment of an element in a row of widgets.
 */
#define OUTSET                                                                 \
    for (int __i = 1, __widget_y_save = __widget_y; __i > 0; --__i)            \
        for (int __j = 1, __widget_y = __widget_y_save - 1; __j > 0; --__j)

/**
 * Start widget rendering.
 *
 * The y argument specifies the vertical position of the next widget, and the
 * horizontal position is set to a standard value.
 */
#define PLACE_AT(y)                                                            \
    for (int __i = 1, __widget_x = 78, __widget_y = y; __i > 0; --__i)

/**
 * Print a text string with the correct offset for visual alignment.
 */
#define PRINT(string)                                                          \
    do {                                                                       \
        graphics_print_small(string, __widget_x + 3, __widget_y, 0, 0, 0);     \
    } while (0)

/**
 * Add a color to already rendered segmented widget.
 */
#define SEGMENT_COLOR(color)                                                   \
    do {                                                                       \
        graphics_draw_from_library(LIBRARY_IMAGE_MENU_S_##color, __widget_x,   \
                                   __widget_y);                                \
    } while (0)

/**
 * Simply flush the screen.
 */
#define UPDATE                                                                 \
    do {                                                                       \
        graphics_update_display();                                             \
    } while (0)

/**
 * Render one of the predefined widgets at the position of the cursor.
 */
#define WIDGET(id)                                                             \
    graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_##id, __widget_x,          \
                               __widget_y)

namespace menu
{

static bool temp_state_led_on = true;
static bool temp_state_sound_music_on = false;
static bool temp_state_sound_sfx_on = true;
static bool temp_state_sound_steps_on = true;
static bool temp_state_wifi_on = true;
static char temp_state_led_pattern[32] = "Rainbow";
static uint8_t temp_state_led_pattern_id = 0;
static uint8_t temp_state_led_brightness = 55;
static uint8_t temp_state_led_color = 3;

int color_id_to_rgb[7] = {0x0000ff, 0x00ff00, 0xffff00, 0xff00ff,
                          0xff0000, 0x00ffff, 0xffffff};
// clang-format off
static void render_led_settings()
{
    MENU(LED);

    PLACE_AT(22)
        temp_state_led_on ? WIDGET(SWITCH_ON)
                          : WIDGET(SWITCH_OFF);

    PLACE_AT(78)
        for (int threshold = 9; threshold < 98; threshold += 10, NEXT_SEGMENT)
            temp_state_led_brightness >= threshold ? WIDGET(SEGMENT_ON)
                                                   : WIDGET(SEGMENT_OFF);

    PLACE_AT(131)
        INSET WIDGET(TEXT);

    PLACE_AT(196)
        for (int color_id = 0; color_id < 7; ++color_id, NEXT_SEGMENT)
            if (color_id != temp_state_led_color)
                WIDGET(SEGMENT_OFF);

            else OUTSET {
                WIDGET(SEGMENT_ON);

                if (color_id == 1)      SEGMENT_COLOR(GREEN);
                else if (color_id == 2) SEGMENT_COLOR(YELLOW);
                else if (color_id == 3) SEGMENT_COLOR(PURPLE);
                else if (color_id == 4) SEGMENT_COLOR(RED);
                else if (color_id == 5) SEGMENT_COLOR(CYAN);
                else if (color_id == 6) SEGMENT_COLOR(WHITE);
            }

    UPDATE;

    PLACE_AT(153)
        PRINT(temp_state_led_pattern);
}
// clang-format on

// clang-format off
static void render_sound_settings()
{
    MENU(SOUND);

    PLACE_AT(37)
        temp_state_sound_music_on ? WIDGET(SWITCH_ON)
                                  : WIDGET(SWITCH_OFF);

    PLACE_AT(76)
        INSET WIDGET(TEXT);

    PLACE_AT(141)
        temp_state_sound_sfx_on ? WIDGET(SWITCH_ON)
                                : WIDGET(SWITCH_OFF);

    PLACE_AT(177)
        temp_state_sound_steps_on ? WIDGET(SWITCH_ON)
                                  : WIDGET(SWITCH_OFF);

    UPDATE;
}
// clang-format on

static char temp_state_wifi_ssid[32] = "";
// clang-format off
static void render_wifi_settings()
{
    MENU(WIFI);

    PLACE_AT(37)
        temp_state_wifi_on ? WIDGET(SWITCH_ON)
                           : WIDGET(SWITCH_OFF);

    PLACE_AT(106)
        INSET WIDGET(TEXT);

    UPDATE;
    PLACE_AT(128)
        PRINT(temp_state_wifi_ssid);

}

// clang-format on

void set_pattern_display_name()
{
    switch (temp_state_led_pattern_id) {
    case 0:
        strncpy(temp_state_led_pattern, "Breath", 7);
        break;
    case 1:
        strncpy(temp_state_led_pattern, "Rainbow", 8);
        break;
    case 2:
        strncpy(temp_state_led_pattern, "Strobe", 7);
        break;
    case 3:
        strncpy(temp_state_led_pattern, "Chase", 6);
        break;
    case 4:
        strncpy(temp_state_led_pattern, "Rain", 5);
        break;
    case 5:
        strncpy(temp_state_led_pattern, "Pride", 6);
        break;
    case 6:
        strncpy(temp_state_led_pattern, "Waves", 6);
        break;
    case 7:
        strncpy(temp_state_led_pattern, "Sparkle", 8);
        break;
    case 8:
        strncpy(temp_state_led_pattern, "Running", 8);
        break;
    case 9:
        strncpy(temp_state_led_pattern, "Rainbow 2", 10);
        break;
    default:
        break;
    }
}

void update_current_neopixel_state()
{
    temp_state_led_brightness =
        int(NeoPixel::getInstance().getBrightNess() / 2.5);
    int color_value = NeoPixel::getInstance().getColor();
    uint8_t color_id;
    for (color_id = 0;
         color_id < sizeof(color_id_to_rgb) / sizeof(color_id_to_rgb[0]);
         color_id++) {
        if (color_value == color_id_to_rgb[color_id]) {
            break;
        }
    }
    temp_state_led_color = color_id;
    temp_state_led_pattern_id = NeoPixel::getInstance().getPublicMode();

    set_pattern_display_name();
}

void display_led_settings()
{
    button_t button;
    update_current_neopixel_state();
    render_led_settings();

    while (true) {
        button = BUTTON_NONE;
        xQueueReceive(button_event_queue, &button, 10 / portTICK_PERIOD_MS);

        switch (button) {
        case BUTTON_BACK_RELEASE:
            Save::write_save();
            // Go back to the menu.
            return;

        case BUTTON_DOWN:
            temp_state_led_color = (temp_state_led_color + 1) % 7;
            break;

        case BUTTON_ENTER:
            temp_state_led_on = !temp_state_led_on;

            break;

        case BUTTON_LEFT:
            temp_state_led_brightness -= 10;
            if (temp_state_led_brightness < 10)
                temp_state_led_brightness = 10;

            break;

        case BUTTON_RIGHT:
            temp_state_led_brightness += 10;
            if (temp_state_led_brightness > 99)
                temp_state_led_brightness = 99;

            break;

        case BUTTON_UP:
            temp_state_led_pattern_id = (temp_state_led_pattern_id + 1) % 10;
            set_pattern_display_name();
            break;

        default:
            continue;
        }

        if (temp_state_led_on) {
            NeoPixel::getInstance().setBrightness(
                int(temp_state_led_brightness * 2.5));
            NeoPixel::getInstance().setColor(
                color_id_to_rgb[temp_state_led_color]);

            NeoPixel::getInstance().setPublicMode(temp_state_led_pattern_id);
        } else {
            NeoPixel::getInstance().setBrightness(0);
        }

        // Refresh the widgets on the display.
        render_led_settings();
    }
}

void display_sound_settings()
{
    button_t button;

    render_sound_settings();

    while (true) {
        button = BUTTON_NONE;
        xQueueReceive(button_event_queue, &button, 10 / portTICK_PERIOD_MS);

        switch (button) {
        case BUTTON_BACK_RELEASE:
            // Go back to the menu.
            return;

        case BUTTON_DOWN:
            temp_state_sound_music_on = false;
            temp_state_sound_steps_on = !temp_state_sound_steps_on;
            break;

        case BUTTON_ENTER:
            temp_state_sound_music_on = !temp_state_sound_music_on;
            temp_state_sound_sfx_on = false;
            temp_state_sound_steps_on = false;
            break;

        case BUTTON_UP:
            temp_state_sound_music_on = false;
            temp_state_sound_sfx_on = !temp_state_sound_sfx_on;
            break;

        default:
            continue;
        }

        // Refresh the widgets on the display.
        render_sound_settings();
    }
}
void update_current_wifi_state()
{
    temp_state_wifi_on = is_wifi_connected();
    if(temp_state_wifi_on) {
        wifi_get_ssid(temp_state_wifi_ssid);
    } else {
        temp_state_wifi_ssid[0] = '\0';
    }
}

void display_wifi_settings()
{
    button_t button;
    update_current_wifi_state();
    render_wifi_settings();

    while (true) {
        button = BUTTON_NONE;
        xQueueReceive(button_event_queue, &button, 10 / portTICK_PERIOD_MS);

        switch (button) {
        case BUTTON_BACK_RELEASE:
            // Go back to the menu.
            return;

        case BUTTON_ENTER:
            temp_state_wifi_on = !temp_state_wifi_on;
            if (temp_state_wifi_on) {
                wifi_join_if_configured();
                wifi_get_ssid(temp_state_wifi_ssid);
            } else {
                wifi_disconnect();
                temp_state_wifi_ssid[0] = '\0';
            }
            break;

        default:
            continue;
        }

        // Refresh the widgets on the display.
        render_wifi_settings();
    }
}

} // namespace menu
