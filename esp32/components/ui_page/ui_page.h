#ifndef SRC_APP_UI_PAGE_H
#define SRC_APP_UI_PAGE_H

#include <stdbool.h>

#include "FreeRTOS.h"
#include "drivers/controls.h"

typedef void (*ui_page_init_func)(void *data);
typedef void (*ui_page_redraw_func)(void);
typedef bool (*ui_page_handle_button_func)(button_t btn);

typedef struct {
    ui_page_init_func init;
    ui_page_redraw_func redraw;
    ui_page_handle_button_func handle_button;
    TickType_t ticks_timeout;
} ui_page;

void show_ui_page(const ui_page *new, void *init_data);

#endif /* SRC_APP_UI_PAGE_H */
