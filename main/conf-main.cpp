/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>
#include "nvs_flash.h"

#include <iostream>

//#include <badge/badge.hpp>
//#include <badge/globals.hpp>
#include <scheduling/task.hpp>

#include "console.h"
#include "dbg-led.hpp"

class dummy_task : public nsec::scheduling::periodic_task<dummy_task>
{
    friend class periodic_task<dummy_task>;

  public:
    dummy_task() : nsec::scheduling::periodic_task<dummy_task>(1000)
    {
        start();
    }

    const char *name() const noexcept
    {
        return "dummy task";
    }

  private:
    void tick(nsec::scheduling::absolute_time_ms current_time_ms
              [[maybe_unused]])
    {
        std::cout << "Hello from dummy task" << std::endl;
    }
};

static void initialize_nvs() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

extern "C" void app_main(void)
{
    initialize_nvs();

    xTaskCreate(dbg_led_task, "dbg_led_task", 2048, nullptr, 1, nullptr);

    //const dummy_task the_task;
    //nsec::runtime::badge badge;
    //nsec::g::the_badge = &badge;

    //badge.start();

    /* Wait a few seconds before enabling the console. */
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    console_init();

    /* Spin to prevent main task from exiting. */
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
