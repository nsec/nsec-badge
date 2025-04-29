// SPDX-FileCopyrightText: 2024 NorthSec
//
// SPDX-License-Identifier: MIT

#include "dock_detector.hpp"
#include <badge/globals.hpp>

namespace nd = nsec::dock;

void nd::dock_detector::apply_sponsor(uint8_t id){
    nsec::g::the_badge->apply_new_sponsor(id);
}

void nd::dock_detector::tick(nsec::scheduling::absolute_time_ms current_time_ms
    [[maybe_unused]])
{
    _dock_detected = check_dock_pins();

    // Check if dock state has changed
    if (_dock_detected != _prev_dock_detected) {
        if (_dock_detected) {
            // Changed from undocked to docked
            std::cout << "Dock detected" << std::endl;
            prev_selected_animation = nsec::g::the_badge->_selected_animation;
            badge_ssd1306_clear();
            vTaskDelay(500/portTICK_PERIOD_MS);
            badge_ssd1306_deinit();
            vTaskDelay(500/portTICK_PERIOD_MS);
            // Initialize I2C slave when dock is detected
            init_i2c_slave();
            
            // Initialize previous data to invalid values to ensure first real data is processed
            prev_data[0] = 0xFF;
            prev_data[1] = 0xFF;
        } else {
            // Changed from docked to undocked
            std::cout << "Dock disconnected" << std::endl;
            
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
            vTaskDelay(500/portTICK_PERIOD_MS);
            nsec::g::the_badge->_set_selected_animation(prev_selected_animation,false,true);
            badge_lcd_nsec_logo();
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
                        
                        std::cout << "I2C data received: ";
                        for (size_t i = 0; i < MSG_SIZE && rx_data.buffer[i] != 0; i++) {
                            std::cout << std::hex << static_cast<int>(rx_data.buffer[i]) << " ";
                        }
                        std::cout << std::dec << std::endl;
                        if(data_rd[0]==0x69){
                            std::cout << "Got magic" << std::endl;
                            apply_sponsor(data_rd[1]);
                        }
                        if(data_rd[0]==0x45){
                            std::cout << "Got color" << std::endl;
                            nsec::g::the_badge->_set_selected_animation(data_rd[1],false,true);
                        }
                    }
                    
                    // Start receiving again (regardless of whether data changed)
                    esp_err_t ret = i2c_slave_receive(_i2c_slave_handle, data_rd, MSG_SIZE);
                    if (ret != ESP_OK) {
                        std::cout << "Failed to restart I2C slave receive: " << esp_err_to_name(ret) << std::endl;
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
        std::cout << "Already initialized" << std::endl;
        return;
    }

    // Configure I2C slave
    i2c_slave_config_t slave_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = static_cast<gpio_num_t>(nsec::board::serial::i2c::i2c_sda),
        .scl_io_num = static_cast<gpio_num_t>(nsec::board::serial::i2c::i2c_scl),
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .send_buf_depth = MSG_SIZE,
        .slave_addr = 0x68,        // Slave address 0x68
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .intr_priority = 0,
        .flags = {.stretch_en = 0 }
    };

    // Create new I2C slave device
    
    esp_err_t ret = i2c_new_slave_device(&slave_config, &_i2c_slave_handle);
    if (ret != ESP_OK) {
        std::cout << "Failed to create I2C slave device: " << esp_err_to_name(ret) << std::endl;
        return;
    }
    
    _receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));
    // Register callback for receiving data
    i2c_slave_event_callbacks_t callbacks = {
        .on_recv_done = i2c_slave_rx_callback,
    };
    ret = i2c_slave_register_event_callbacks(_i2c_slave_handle, &callbacks, _receive_queue);
    if (ret != ESP_OK) {
        std::cout << "Failed to register I2C slave callbacks: " << esp_err_to_name(ret) << std::endl;
        i2c_del_slave_device(_i2c_slave_handle);
        _i2c_slave_handle = nullptr;
        return;
    }

    // Start receiving data
    // uint8_t* rx_buffer = new uint8_t[64];  // Buffer for receiving data
    ret = i2c_slave_receive(_i2c_slave_handle, data_rd, MSG_SIZE);
    if (ret != ESP_OK) {
        std::cout << "Failed to start I2C slave receive: " << esp_err_to_name(ret) << std::endl;
        i2c_del_slave_device(_i2c_slave_handle);
        _i2c_slave_handle = nullptr;
        return;
    }

    std::cout << "I2C slave initialized on address 0x69" << std::endl;
}

bool nd::dock_detector::check_dock_pins()
{
    // Check if pins 2 and 6 are HIGH
    bool pin2_high = gpio_get_level(GPIO_NUM_2) == 1;
    bool pin6_high = gpio_get_level(GPIO_NUM_6) == 1;
    
    // Badge is docked if pins 2 and 6 are HIGH
    return pin2_high && pin6_high;
}