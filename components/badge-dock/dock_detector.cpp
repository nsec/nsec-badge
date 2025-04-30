// SPDX-FileCopyrightText: 2024 NorthSec
//
// SPDX-License-Identifier: MIT

#include "dock_detector.hpp"
#include <badge/globals.hpp>

namespace nb = nsec::board;
namespace nd = nsec::dock;
namespace ng = nsec::g;

nd::dock_detector::dock_detector() : 
    nsec::scheduling::periodic_task<dock_detector>(200), 
    _logger("dock detector", nsec::config::logging::dock_detector_level)
{
    _dock_detected = false;
    _prev_dock_detected = false;
    
    // Initialize GPIO pins 2 and 6 as inputs
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << nb::dock::input1_pin) | (1ULL << nb::dock::input2_pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    // Configure pin 7 as output and set it HIGH
    gpio_config_t output_conf = {};
    output_conf.intr_type = GPIO_INTR_DISABLE;
    output_conf.mode = GPIO_MODE_OUTPUT;
    output_conf.pin_bit_mask = (1ULL << nb::dock::output_pin);
    output_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    output_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&output_conf);
    
    // Set pin 7 to HIGH
    gpio_set_level(static_cast<gpio_num_t>(nb::dock::output_pin), 1);
    
}


void nd::dock_detector::tick(nsec::scheduling::absolute_time_ms current_time_ms
    [[maybe_unused]])
{
    _dock_detected = check_dock_pins();

    // Check if dock state has changed
    if (_dock_detected != _prev_dock_detected) {
        if (_dock_detected) {
            // Changed from undocked to docked
            _logger.debug("Dock connected");
            ng::the_badge->apply_dock_status(_dock_detected);
            
            // Initialize I2C slave when dock is detected
            init_i2c_slave();
            
            // Initialize previous data to invalid values to ensure first real data is processed
            prev_data[0] = 0xFF;
            prev_data[1] = 0xFF;
        } else {
            // Changed from docked to undocked
            _logger.debug("Dock disconnected");
            
            // Clean up I2C slave when dock is disconnected
            if (_i2c_slave_handle != nullptr) {
                i2c_del_slave_device(_i2c_slave_handle);
                _i2c_slave_handle = nullptr;
            }
            
            // Clean up the receive queue
            if (_receive_queue != nullptr) {
                vQueueDelete(_receive_queue);
                _receive_queue = nullptr;
            }

            ng::the_badge->apply_dock_status(_dock_detected);
        }

        // Update previous state
        _prev_dock_detected = _dock_detected;
    }

    // Check the contents of the receive queue on every tick
    if (_receive_queue != nullptr) {
        if (xQueueReceive(_receive_queue, &rx_data,0) == pdTRUE) {
            // Process the received data
            if (rx_data.buffer != nullptr) {
                if(data_rd[0]!=0xff && data_rd[1]!=0xff){
                    // Check if data is different from previously received data
                    bool data_changed = (data_rd[0] != prev_data[0]) || (data_rd[1] != prev_data[1]);
                    
                    if (data_changed) {
                        // Store current data as previous data for next comparison
                        prev_data[0] = data_rd[0];
                        prev_data[1] = data_rd[1];
                        
                        _logger.debug("I2C data received: {:x} {:x}",data_rd[0], data_rd[1]);

                        ng::the_badge->apply_i2c_command(data_rd[0], data_rd[1]);
                    }
                    
                    // Start receiving again (regardless of whether data changed)
                    esp_err_t ret = i2c_slave_receive(_i2c_slave_handle, data_rd, MSG_SIZE);
                    if (ret != ESP_OK) {
                        _logger.error("Failed to restart I2C slave receive: {}", esp_err_to_name(ret));
                    }
                }
            }
        }
    }
}


void nd::dock_detector::init_i2c_slave()
{
    if (_i2c_slave_handle != nullptr) {
        // Already initialized
        _logger.debug("I2C Slave Already initialized");
        return;
    }

    // Configure I2C slave
    i2c_slave_config_t slave_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = static_cast<gpio_num_t>(nsec::board::serial::i2c::i2c_sda),
        .scl_io_num = static_cast<gpio_num_t>(nsec::board::serial::i2c::i2c_scl),
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .send_buf_depth = MSG_SIZE,
        .slave_addr = nsec::board::serial::i2c::i2c_slave_addr,
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .intr_priority = 0,
        .flags = {.stretch_en = 0 }
    };

    // Create new I2C slave device
    
    esp_err_t ret = i2c_new_slave_device(&slave_config, &_i2c_slave_handle);
    if (ret != ESP_OK) {
        _logger.error("Failed to create I2C slave device: ", esp_err_to_name(ret));
        return;
    }
    
    _receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));
    // Register callback for receiving data
    i2c_slave_event_callbacks_t callbacks = {
        .on_recv_done = i2c_slave_rx_callback,
    };
    ret = i2c_slave_register_event_callbacks(_i2c_slave_handle, &callbacks, _receive_queue);
    if (ret != ESP_OK) {
        _logger.error("Failed to register I2C slave callbacks: ", esp_err_to_name(ret));
        i2c_del_slave_device(_i2c_slave_handle);
        _i2c_slave_handle = nullptr;
        return;
    }

    // Start receiving data
    // uint8_t* rx_buffer = new uint8_t[64];  // Buffer for receiving data
    ret = i2c_slave_receive(_i2c_slave_handle, data_rd, MSG_SIZE);
    if (ret != ESP_OK) {
        _logger.error("Failed to start I2C slave receive: ", esp_err_to_name(ret));
        i2c_del_slave_device(_i2c_slave_handle);
        _i2c_slave_handle = nullptr;
        return;
    }

    _logger.debug("I2C slave initialized");
}

bool nd::dock_detector::check_dock_pins()
{
    // Check if pins 2 and 6 are HIGH
    bool input1_high = gpio_get_level(static_cast<gpio_num_t>(nb::dock::input1_pin)) == 1;
    bool input2_high = gpio_get_level(static_cast<gpio_num_t>(nb::dock::input2_pin)) == 1;
    
    // Badge is docked if pins 2 and 6 are HIGH
    return input1_high && input2_high;
}
