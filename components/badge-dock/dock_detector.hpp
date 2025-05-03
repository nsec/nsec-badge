/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2025 NorthSec
 */

#ifndef NSEC_DOCK_DETECTOR_HPP
#define NSEC_DOCK_DETECTOR_HPP

#include "scheduling/task.hpp"
#include "utils/board.hpp"
#include "utils/logging.hpp"

#include <cstring>
#include <iostream>
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c_slave.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ssd1306/badge_ssd1306_helper.hpp"
#include "ssd1306/badge_nsec_logo.h"

#define MSG_SIZE 2         // Buffer Length

// SemaphoreHandle_t xSemaphoreSetI2C;
namespace nsec::dock
{

    class dock_detector : public nsec::scheduling::periodic_task<dock_detector>
    {
        friend class nsec::scheduling::periodic_task<dock_detector>;

        public:
        bool _dock_detected;
        dock_detector();
        
        void start()
        {
            scheduling::periodic_task<dock_detector>::start();
        }

        const char *name() const noexcept
        {
            return "dock detect";
        }

        // I2C slave callback function
        static bool i2c_slave_rx_callback(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)
        {
            BaseType_t high_task_wakeup = pdFALSE;
            QueueHandle_t receive_queue = (QueueHandle_t)arg;
            xQueueSendFromISR(receive_queue, evt_data, &high_task_wakeup);
            return high_task_wakeup == pdTRUE;
        }
        
        void init_i2c_slave();

        private:
        bool check_dock_pins();
        
        void tick(nsec::scheduling::absolute_time_ms current_time_ms
                    [[maybe_unused]]);

        bool _prev_dock_detected;
        i2c_slave_dev_handle_t _i2c_slave_handle = nullptr;
        QueueHandle_t _receive_queue = nullptr;
        uint8_t data_rd[MSG_SIZE] = {0};
        uint32_t size_rd = 0;
        i2c_slave_rx_done_event_data_t rx_data;
        uint8_t command_rx[MSG_SIZE] = {0};
        uint8_t prev_data[MSG_SIZE] = {0xFF, 0xFF}; // Store previous I2C data for comparison
        nsec::logging::logger _logger;
    };

    // Implementation of the I2C slave callback function


} // namespace nsec::dock
#endif // NSEC_DOCK_DETECTOR_HPP
