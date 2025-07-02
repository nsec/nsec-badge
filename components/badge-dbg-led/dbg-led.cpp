// SPDX-FileCopyrightText: 2025 NorthSec
//
// SPDX-License-Identifier: MIT

#include "dbg-led.hpp"

#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define LEDC_OUTPUT_IO 13

/*
 * This callback function will be called when fade operation has ended
 * Use callback only if you are aware it is being called inside an ISR
 * Otherwise, you can use a semaphore to unblock tasks
 */
static IRAM_ATTR bool cb_ledc_fade_end_event(const ledc_cb_param_t *param,
                                             void *user_arg)
{
    BaseType_t taskAwoken = pdFALSE;

    if (param->event == LEDC_FADE_END_EVT) {
        SemaphoreHandle_t counting_sem = (SemaphoreHandle_t)user_arg;
        xSemaphoreGiveFromISR(counting_sem, &taskAwoken);
    }

    return (taskAwoken == pdTRUE);
}

void dbg_led_task(void *arg __attribute__((unused)))
{
    // Prepare and then apply the LEDC PWM timer configuration
    const ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,    // Low speed mode
        .duty_resolution = LEDC_TIMER_13_BIT, // Set duty resolution to 13 bits
        .timer_num = LEDC_TIMER_0,            // Use timer 0
        .freq_hz = 4000,                      // Set output frequency at 4 kHz
        .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
    };
    ledc_timer_config(&ledc_timer);

    // Prepare and then apply the LEDC PWM channel configuration
    const ledc_channel_config_t ledc_channel = {
        .gpio_num = LEDC_OUTPUT_IO,        // GPIO number for the LED
        .speed_mode = LEDC_LOW_SPEED_MODE, // Low speed mode
        .channel = LEDC_CHANNEL_0,         // Use channel 0
        .intr_type = LEDC_INTR_DISABLE,    //
        .timer_sel = LEDC_TIMER_0,         // Use timer 0
        .duty = 0,                         // Set initial duty to 0 / off
        .hpoint = 0                        //
    };
    ledc_channel_config(&ledc_channel);

    // Initialize fade service.
    ledc_fade_func_install(0);
    ledc_cbs_t callbacks = {.fade_cb = cb_ledc_fade_end_event};

    SemaphoreHandle_t counting_sem = xSemaphoreCreateCounting(1, 0);
    ledc_cb_register(ledc_channel.speed_mode, ledc_channel.channel, &callbacks,
                     (void *)counting_sem);

    // Loop forever
    while (true) {
        // 2 quick beats of 1 second each
        for (int i = 0; i < 2; i++) {
            // Fade from 0 to 4096, over 500ms
            ledc_set_fade_with_time(ledc_channel.speed_mode,
                                    ledc_channel.channel, 4096, 500);
            ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel,
                            LEDC_FADE_NO_WAIT);

            // Wait for the fade to complete
            xSemaphoreTake(counting_sem, portMAX_DELAY);

            // Fade from 4096 to 0, over 500 msec
            ledc_set_fade_with_time(ledc_channel.speed_mode,
                                    ledc_channel.channel, 0, 500);
            ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel,
                            LEDC_FADE_NO_WAIT);

            // Wait for the fade to complete
            xSemaphoreTake(counting_sem, portMAX_DELAY);
        }

        // Wait for one second
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
