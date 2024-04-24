/* SPDX-FileCopyrightText: 2024 NorthSec
 *
 * SPDX-License-Identifier: MIT */

/******************************************************************************
 *
 * Description: Badge Button Processing.
 *              - Espressif IOT_BUTTON interface is used to get the
 *                different button events.
 *                1) This interface handle the button debouncing.
 *              - The badge buttons process only handle the "Single Click" &
 *                the "Long Press Event" from the "IOT Button".
 *                1) All other event are ignored.
 *                2) In the current implementation, the "Double Click" &
 *                   "Multiple Click" events is ignored.
 *
 * Process:
 * - Setup all events callback for the differents buttons.
 * - Process all received callback event and mapped to badge button
     events (Single Click & Long Press) or ignored the events.
 * - Send the event to the registered callback function
 *   (on the received callback itself).
 * 
 * Reference:
 * - Espressif IOT Button documentation & API
 *  https://docs.espressif.com/projects/esp-iot-solution/en/latest/input_device/button.html
 *
 *****************************************************************************/
 
#include "buttons.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <esp_log.h>
#include <iot_button.h>
#include <sdkconfig.h>

/* Local debugging options. */
//#define DEBUG_BUTTON_MAPPED
//#define DEBUG_BUTTON_UNMAPPED
//#define DEBUG_BUTTON_CALLBACK

/* Buttons */
#define GPIO_BTN_UP     (35)
#define GPIO_BTN_DOWN   (36)
#define GPIO_BTN_LEFT   (37)
#define GPIO_BTN_RIGHT  (38)
#define GPIO_BTN_OK     (39)
#define GPIO_BTN_CANCEL (40)

static void up_button_cb( void *button_handle, void *data );
static void down_button_cb( void *button_handle, void *data );
static void left_button_cb( void *button_handle, void *data );
static void right_button_cb( void *button_handle, void *data );
static void ok_button_cb( void *button_handle, void *data );
static void cancel_button_cb( void *button_handle, void *data );
static void button_cb_process( badge_button_t button, void *data );

static const char *TAG = "BUTTON";

const char *button_event_table[] = {
    "PRESS_DOWN",
    "PRESS_UP",
    "PRESS_REPEAT",
    "PRESS_REPEAT_DONE",
    "SINGLE_CLICK",
    "DOUBLE_CLICK",
    "MULTIPLE_CLICK",
    "LONG_PRESS_START",
    "LONG_PRESS_HOLD",
    "LONG_PRESS_UP"
};

const char *button_label_table[] = {
    "UP",
    "DOWN",
    "LEFT",    
    "RIGHT",
    "OK",
    "CANCEL"
};

#ifdef DEBUG_BUTTON_CALLBACK
const char *badge_button_event_table[] = {
    "SINGLE_CLICK",
    "LONG_PRESS"
};
#endif

/* Mapping IOT buttons event to badge buttons event.
 * - Ignored events are mapped to "BADGE_BUTTON_TOTAL_COUNT".
 */
static badge_button_event_t button_event_mapping[BUTTON_EVENT_MAX] = {
    BADGE_BUTTON_TOTAL_COUNT,   // BUTTON_PRESS_DOWN
    BADGE_BUTTON_TOTAL_COUNT,   // BUTTON_PRESS_UP
    BADGE_BUTTON_TOTAL_COUNT,   // BUTTON_PRESS_REPEAT
    BADGE_BUTTON_TOTAL_COUNT,   // BUTTON_PRESS_REPEAT_DONE
    BADGE_BUTTON_SINGLE_CLICK,  // BUTTON_SINGLE_CLICK
    BADGE_BUTTON_TOTAL_COUNT,   // BUTTON_DOUBLE_CLICK
    BADGE_BUTTON_TOTAL_COUNT,   // BUTTON_MULTIPLE_CLICK
    BADGE_BUTTON_TOTAL_COUNT,   // BUTTON_LONG_PRESS_START
    BADGE_BUTTON_TOTAL_COUNT,   // BUTTON_LONG_PRESS_HOLD,
    BADGE_BUTTON_LONG_PRESS     // BUTTON_LONG_PRESS_UP
};

static button_config_t gpio_btn_cfg = {
    .type = BUTTON_TYPE_GPIO,
    .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
    .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
    .gpio_button_config = {
        .gpio_num = 0,
        .active_level = 0,
        #if CONFIG_GPIO_BUTTON_SUPPORT_POWER_SAVE
        .enable_power_save = true,
        #endif
    },
};

static button_handle_t gpio_btn;
static int gpio_buttons[] = { GPIO_BTN_UP, GPIO_BTN_DOWN, GPIO_BTN_LEFT,
                              GPIO_BTN_RIGHT, GPIO_BTN_OK, GPIO_BTN_CANCEL };
static button_cb_t cb_button_functions[] = {
    up_button_cb,
    down_button_cb,
    left_button_cb,
    right_button_cb,
    ok_button_cb,
    cancel_button_cb
};
static badge_button_event_cb_t cb_button_event = NULL;

/* "UP" button callback function for the different events. */
static void up_button_cb( void *button_handle, void *data )
{
    button_cb_process( BUTTON_UP, data);
}

/* "DOWN" button callback function for the different events. */
static void down_button_cb( void *button_handle, void *data )
{
    button_cb_process( BUTTON_DOWN, data);
}

/* "LEFT" button callback function for the different events. */
static void left_button_cb( void *button_handle, void *data )
{
    button_cb_process( BUTTON_LEFT, data);
}

/* "RIGHT" button callback function for the different events. */
static void right_button_cb( void *button_handle, void *data )
{
    button_cb_process( BUTTON_RIGHT, data);
}

/* "OK" button callback function for the different events. */
static void ok_button_cb( void *button_handle, void *data )
{
    button_cb_process( BUTTON_OK, data);
}

/* "CANCEL" button callback function for the different events. */
static void cancel_button_cb( void *button_handle, void *data )
{
    button_cb_process( BUTTON_CANCEL, data);
}

/* Button  processing function for the different callback events. */
static void button_cb_process( badge_button_t button, void *data)
{
    /* Only process certain events. */
    switch((button_event_t)data)
    {
        case BUTTON_SINGLE_CLICK:
        case BUTTON_LONG_PRESS_UP:
            #ifdef DEBUG_BUTTON_MAPPED
            ESP_LOGI(TAG, "%s: %s", button_label_table[button],
                                    button_event_table[(button_event_t)data]);
            #endif
            if(cb_button_event != NULL)
            {
                cb_button_event( button, button_event_mapping[(button_event_t)data]);
            }    
            break;
        
        default:
            #ifdef DEBUG_BUTTON_UNMAPPED
            ESP_LOGI(TAG, "%s: %s", button_label_table[button],
                                    button_event_table[(button_event_t)data]);
            #endif
            break;
    }            
}

#ifdef DEBUG_BUTTON_CALLBACK
static void badge_button_event_cb_process( badge_button_t button,
                                           badge_button_event_t event
                                         )
{
    /* Log button event on serial console. */
    ESP_LOGI( "BUTTON EVENT", "%s: %s", button_label_table[button],
                                        badge_button_event_table[event]);
}
#endif

/******************************************************************************
 * Description: Badge buttons initialization.
 *
 * param: None.
 *
 * return: None.
 *
 *****************************************************************************/
 void buttons_init( void )
{
    for (int i = 0; i < BUTTON_TOTAL_COUNT; i++)
    {
        gpio_btn_cfg.gpio_button_config.gpio_num = gpio_buttons[i];

        gpio_btn = iot_button_create(&gpio_btn_cfg);
        if (gpio_btn == NULL) {
            ESP_LOGE(TAG, "Button create failed");
        }

        iot_button_register_cb( gpio_btn, BUTTON_PRESS_DOWN,
                                cb_button_functions[i],
                                (void *)BUTTON_PRESS_DOWN);
        iot_button_register_cb( gpio_btn, BUTTON_PRESS_UP,
                                cb_button_functions[i],
                                (void *)BUTTON_PRESS_UP);
        iot_button_register_cb( gpio_btn, BUTTON_PRESS_REPEAT,
                                cb_button_functions[i],
                                (void *)BUTTON_PRESS_REPEAT);
        iot_button_register_cb( gpio_btn, BUTTON_PRESS_REPEAT_DONE,
                                cb_button_functions[i],
                                (void *)BUTTON_PRESS_REPEAT_DONE);
        iot_button_register_cb( gpio_btn, BUTTON_SINGLE_CLICK, 
                                cb_button_functions[i],
                                (void *)BUTTON_SINGLE_CLICK);
        iot_button_register_cb( gpio_btn, BUTTON_DOUBLE_CLICK, 
                                cb_button_functions[i],
                                (void *)BUTTON_DOUBLE_CLICK);
        iot_button_register_cb( gpio_btn, BUTTON_MULTIPLE_CLICK, 
                                cb_button_functions[i],
                                (void *)BUTTON_MULTIPLE_CLICK);
        iot_button_register_cb( gpio_btn, BUTTON_LONG_PRESS_START, 
                                cb_button_functions[i],
                                (void *)BUTTON_LONG_PRESS_START);
        iot_button_register_cb( gpio_btn, BUTTON_LONG_PRESS_HOLD, 
                                cb_button_functions[i],
                                (void *)BUTTON_LONG_PRESS_HOLD);
        iot_button_register_cb( gpio_btn, BUTTON_LONG_PRESS_UP,
                                cb_button_functions[i],
                                (void *)BUTTON_LONG_PRESS_UP);
    }

    #ifdef DEBUG_BUTTON_CALLBACK
    buttons_register_ca(badge_button_event_cb_process);
    #endif
}

/******************************************************************************
 * Description: Register callback badge buttons event.
 *
 * param: Badge button event callback.
 *
 * return: None.
 *
 *****************************************************************************/
void buttons_register_ca( badge_button_event_cb_t cb )
{
    cb_button_event = cb;
}
