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
        dock_detector() : nsec::scheduling::periodic_task<dock_detector>(200)
        {
            _dock_detected = false;
            _prev_dock_detected = false;
            
            // Initialize GPIO pins 2 and 6 as inputs
            gpio_config_t io_conf = {};
            io_conf.intr_type = GPIO_INTR_DISABLE;
            io_conf.mode = GPIO_MODE_INPUT;
            io_conf.pin_bit_mask = (1ULL << 2) | (1ULL << 6);
            io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
            io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
            gpio_config(&io_conf);
            
            // Configure pin 7 as output and set it HIGH
            gpio_config_t output_conf = {};
            output_conf.intr_type = GPIO_INTR_DISABLE;
            output_conf.mode = GPIO_MODE_OUTPUT;
            output_conf.pin_bit_mask = (1ULL << 7);
            output_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
            output_conf.pull_up_en = GPIO_PULLUP_DISABLE;
            gpio_config(&output_conf);
            
            // Set pin 7 to HIGH
            gpio_set_level(GPIO_NUM_7, 1);
            
            // start();
        }
        
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
        
        void apply_sponsor(uint8_t id);

        bool _prev_dock_detected;
        i2c_slave_dev_handle_t _i2c_slave_handle = nullptr;
        QueueHandle_t _receive_queue = nullptr;
        uint8_t *data_rd = (uint8_t *) malloc(MSG_SIZE);
        uint32_t size_rd = 0;
        i2c_slave_rx_done_event_data_t rx_data;
        uint8_t command_rx[MSG_SIZE] = {0};
        uint8_t prev_selected_animation = 0;
        uint8_t prev_data[MSG_SIZE] = {0xFF, 0xFF}; // Store previous I2C data for comparison

    };

    // Implementation of the I2C slave callback function


} // namespace nsec::dock
#endif // NSEC_DOCK_DETECTOR_HPP

