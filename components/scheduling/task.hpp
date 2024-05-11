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

#include <utils/config.hpp>

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
     *
     * Users must define a `tick` method that is periodically invoked by the
     * scheduler:
     *   void tick(nsec::scheduling::absolute_time_ms current_time_ms);
     *
     * To set a custom task name, users must define a name() method:
     *   const char *name() const noexcept;
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

    const char *name() const noexcept
    {
        return "Unnamed task";
    }

  protected:
    // Call once derived task is fully initialized.
    void start_with_prio(int priority)
    {
        const auto result = xTaskCreatePinnedToCore(
            _tick_and_wait, name(),
            nsec::config::scheduling::default_stack_size_words, (void *)this,
            priority, &_handle, 0);

        if (result != pdPASS) {
            throw std::bad_alloc();
        }
    }

    void start()
    {
	    start_with_prio(nsec::config::scheduling::default_task_priority);
    }

  private:
    static void _tick_and_wait(void *task_ptr)
    {
        auto &task = *static_cast<UserTask *>(task_ptr);

        while (true) {
            try {
                task.tick(xTaskGetTickCount() * portTICK_PERIOD_MS);
            } catch (const std::exception &ex) {
                nsec::logging::logger exception_logger("Task exception handler");

                exception_logger.error("{}", ex.what());
                vTaskDelay(1000 / portTICK_PERIOD_MS);

                // Die with an unhandled exception.
                throw ex;
            }
            vTaskDelay(task._period_ticks);
        }
    }

    relative_time_ms _period_ms;
    TickType_t _period_ticks;
    TaskHandle_t _handle;
};

} // namespace nsec::scheduling

#endif /* NSEC_SCHEDULING_TASK_HPP */
