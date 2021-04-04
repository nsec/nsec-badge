#include "menu.h"

#include "buttons.h"
#include "graphics.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace menu
{

static bool temp_state_led_on = true;
static bool temp_state_sound_music_on = false;;
static bool temp_state_sound_sfx_on = true;;
static bool temp_state_sound_steps_on = true;
static bool temp_state_wifi_on = true;
static char temp_state_led_pattern[32] = "Rainbow";
static int temp_state_led_brightness = 55;
static int temp_state_led_color = 3;

static constexpr int menu_widget_x_text = 74;
static constexpr int menu_widget_x_widget = 78;

static void render_led_settings()
{
    graphics_draw_from_library(LIBRARY_IMAGE_MENU_B_LED, 0, 0);

    if (temp_state_led_on) {
        graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SWITCH_ON,
                                   menu_widget_x_widget, 22);
    } else {
        graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SWITCH_OFF,
                                   menu_widget_x_widget, 22);
    }

    graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_TEXT, menu_widget_x_text,
                               131);

    for (int i = 0; i < 9; ++i) {
        int threshold = i * 10 + 9;
        int widget_x = menu_widget_x_widget + (15 * i);

        if (temp_state_led_brightness >= threshold) {
            graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SEGMENT_ON,
                                       widget_x, 78);
        } else {
            graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SEGMENT_OFF,
                                       widget_x, 78);
        }
    }

    for (int i = 0; i < 7; ++i) {
        int widget_x = menu_widget_x_widget + (15 * i);
        int widget_y = 196;

        if (i == temp_state_led_color) {
            graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SEGMENT_ON,
                                       widget_x, widget_y - 1);

            switch (i) {
            case 0:
                // The base widget is already blue.
                break;

            case 1:
                graphics_draw_from_library(LIBRARY_IMAGE_MENU_S_GREEN, widget_x,
                                           widget_y - 1);
                break;

            case 2:
                graphics_draw_from_library(LIBRARY_IMAGE_MENU_S_YELLOW,
                                           widget_x, widget_y - 1);
                break;

            case 3:
                graphics_draw_from_library(LIBRARY_IMAGE_MENU_S_PURPLE,
                                           widget_x, widget_y - 1);
                break;

            case 4:
                graphics_draw_from_library(LIBRARY_IMAGE_MENU_S_RED, widget_x,
                                           widget_y - 1);
                break;

            case 5:
                graphics_draw_from_library(LIBRARY_IMAGE_MENU_S_CYAN, widget_x,
                                           widget_y - 1);
                break;

            case 6:
                graphics_draw_from_library(LIBRARY_IMAGE_MENU_S_WHITE, widget_x,
                                           widget_y - 1);
                break;
            }
        } else {
            graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SEGMENT_OFF,
                                       widget_x, widget_y);
        }
    }

    graphics_update_display();

    graphics_print_small(temp_state_led_pattern, 80, 153, 0, 0, 0);
}

static void render_sound_settings()
{
    graphics_draw_from_library(LIBRARY_IMAGE_MENU_B_SOUND, 0, 0);

    if (temp_state_sound_music_on) {
        graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SWITCH_ON,
                                   menu_widget_x_widget, 37);
    } else {
        graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SWITCH_OFF,
                                   menu_widget_x_widget, 37);
    }

    graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_TEXT, menu_widget_x_text,
                               76);

    if (temp_state_sound_sfx_on) {
        graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SWITCH_ON,
                                   menu_widget_x_widget, 141);
    } else {
        graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SWITCH_OFF,
                                   menu_widget_x_widget, 141);
    }

    if (temp_state_sound_steps_on) {
        graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SWITCH_ON,
                                   menu_widget_x_widget, 177);
    } else {
        graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SWITCH_OFF,
                                   menu_widget_x_widget, 177);
    }

    graphics_update_display();
}

static void render_wifi_settings()
{
    graphics_draw_from_library(LIBRARY_IMAGE_MENU_B_WIFI, 0, 0);

    graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_TEXT, menu_widget_x_text,
                               106);

    if (temp_state_wifi_on) {
        graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SWITCH_ON,
                                   menu_widget_x_widget, 37);
    } else {
        graphics_draw_from_library(LIBRARY_IMAGE_MENU_W_SWITCH_OFF,
                                   menu_widget_x_widget, 37);
    }

    graphics_update_display();
}

void display_led_settings()
{
    button_t button;

    render_led_settings();

    while (true) {
        button = BUTTON_NONE;
        xQueueReceive(button_event_queue, &button, 10 / portTICK_PERIOD_MS);

        switch (button) {
        case BUTTON_BACK_RELEASE:
            // Go back to the menu.
            return;

        case BUTTON_DOWN:
            // Change LED mode.
            break;

        case BUTTON_ENTER:
            // Switch LED on/off.
            break;

        case BUTTON_LEFT:
            // Decrease LED brightness.
            break;

        case BUTTON_RIGHT:
            // Increase LED brightness.
            break;

        case BUTTON_UP:
            // Change LED mode.
            break;

        default:
            continue;
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

        case BUTTON_ENTER:
            // Switch sound on/off.
            break;

        default:
            continue;
        }

        // Refresh the widgets on the display.
        render_sound_settings();
    }
}

void display_wifi_settings()
{
    button_t button;

    render_wifi_settings();

    while (true) {
        button = BUTTON_NONE;
        xQueueReceive(button_event_queue, &button, 10 / portTICK_PERIOD_MS);

        switch (button) {
        case BUTTON_BACK_RELEASE:
            // Go back to the menu.
            return;

        case BUTTON_ENTER:
            // Switch WiFi on/off.
            break;

        default:
            continue;
        }

        // Refresh the widgets on the display.
        render_wifi_settings();
    }
}

} // namespace menu
