/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_SCHEDULING_TASK_HPP
#define NSEC_SCHEDULING_TASK_HPP

#include "time.hpp"

#include <iostream>
#include <stddef.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace nsec::scheduling
{

template <class UserTask> class periodic_task
{
  public:
    /*
     * Periodic task are automatically rescheduled following their period.
     * Note that the scheduler cannot guarantee the deadlines are honored.
     * As such, a task that couldn't be run at its set period will only run
     * once even if the period was exceeded multiple times.
     *
     * A periodic task will also be re-queued at current_time + period_ms
     * which can cause the timing of tasks to "drift" when deadlines are
     * not honored. If a task needs to be invoked at a precise time, use
     * a regular task and enqueue it manually by providing a relative time
     * as the deadline.
     */
    explicit periodic_task(relative_time_ms task_period_ms) noexcept
    {
        period_ms(task_period_ms);
    }

    /* Deactivate copy and assignment. */
    periodic_task(const periodic_task &) = delete;
    periodic_task(periodic_task &&) = delete;
    periodic_task &operator=(const periodic_task &) = delete;
    periodic_task &operator=(periodic_task &&) = delete;
    virtual ~periodic_task() = default;

    relative_time_ms period_ms() const noexcept
    {
        return _period_ms;
    }

    // Effective at the end of the next tick.
    void period_ms(relative_time_ms new_period_ms) noexcept
    {
        _period_ms = new_period_ms;
        _period_ticks = new_period_ms / portTICK_PERIOD_MS;
    }

  protected:
    // Call once derived task is fully initialized.
    void start()
    {
        const auto result = xTaskCreate(_tick_and_wait, "NAME", 4096,
                                        (void *)this, 3, &_handle);

        if (result != pdPASS) {
            throw std::bad_alloc();
        }
    }

    // Periodic task tick.
    void tick(scheduling::absolute_time_ms current_time_ms)
    {
    }

  private:
    static void _tick_and_wait(void *task_ptr)
    {
        auto &task = *static_cast<UserTask *>(task_ptr);

        while (true) {
            task.tick(0);
            vTaskDelay(task._period_ticks);
        }
    }

    relative_time_ms _period_ms;
    TickType_t _period_ticks;
    TaskHandle_t _handle;
};

} // namespace nsec::scheduling

#endif /* NSEC_SCHEDULING_TASK_HPP */