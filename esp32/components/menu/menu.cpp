#include "menu.h"

#include "buttons.h"
#include "graphics.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace menu
{

static void render_led_settings()
{
    graphics_update_display();

    // Render the widgets in the correct state, text, etc.
    // ...
}

static void render_sound_settings()
{
    graphics_update_display();

    // Render the widgets in the correct state, text, etc.
    // ...
}

static void render_wifi_settings()
{
    graphics_update_display();

    // Render the widgets in the correct state, text, etc.
    // ...
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
