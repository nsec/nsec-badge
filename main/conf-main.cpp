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

#include <iostream>

#include <badge/badge.hpp>
#include <badge/globals.hpp>
#include <scheduling/task.hpp>

#include "console.h"
#include "ota_init.h"

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

extern "C" void app_main(void)
{
    // Detect CTF Addon
    ota_init();
    
    //const dummy_task the_task;
    nsec::runtime::badge badge;
    nsec::g::the_badge = &badge;

    badge.start();

    /* Wait a few seconds before enabling the console. */
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    console_init();

    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
