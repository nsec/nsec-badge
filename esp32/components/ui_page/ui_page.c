//  Copyright (c) 2017
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <string.h>

#include "buttons.h"
#include "ui_page.h"

static const ui_page *top_most_page = NULL;

static void flush_buttons(void)
{
    BaseType_t ret;

    do {
        button_t btn;
        ret = xQueueReceive(button_event_queue, &btn, 0);
    } while (ret == pdTRUE);
}

void show_ui_page(const ui_page *new, void *init_data)
{
    const ui_page *prev_page = top_most_page;
    bool quit = false;
    TickType_t ticks_timeout = portMAX_DELAY;

    top_most_page = new;

    /* For us, ticks_timeout of 0 means no timeout. */
    if (new->ticks_timeout > 0) {
        ticks_timeout = new->ticks_timeout;
    }

    /* Clear any pending button event from the previous page. */
    flush_buttons();

    /* Can't have init data if no init. */
    assert(new->init || !init_data);

    if (new->init) {
        new->init(init_data);
    }

    new->redraw();

    while (!quit) {
        button_t btn;

        BaseType_t ret = xQueueReceive(button_event_queue, &btn, ticks_timeout);

        if (ret == pdTRUE) {
            quit = new->handle_button(btn);
        } else {
            /* The receive timed out. */
            quit = new->handle_button(BUTTON_NONE);
        }
    }

    /* Clear any pending button event from the page we quit. */
    flush_buttons();

    /* The initial page is not allowed to quit / return. */
    assert(prev_page != NULL);

    top_most_page = prev_page;

    /* Redraw previous page. */
    prev_page->redraw();
}
