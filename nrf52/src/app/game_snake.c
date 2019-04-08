#include "drivers/controls.h"

#include "application.h"

#define SNAKE_BUTTON_NONE 255

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

void snake_application(void (*service_device)())
{
    nsec_controls_add_handler(snake_buttons_handle);

    while (application_get() == snake_application) {
        service_device();
    }

    nsec_controls_suspend_handler(snake_buttons_handle);
}
