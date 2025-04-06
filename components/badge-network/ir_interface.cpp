/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2025 Abdelhakim Qbaich <abdelhakim@qbaich.com>
 */

#include "ir_interface.hpp"
#include <memory>

namespace nc = nsec::communication;

#define NSEC_THROW_COMMUNICATION_ERROR(msg)                                    \
    throw nsec::communication::exception::communication_error(                 \
        msg, __FILE__, __func__, __LINE__)
#define NSEC_THROW_PROTOCOL_ERROR(msg)                                         \
    throw nsec::communication::exception::protocol_error(msg, __FILE__,        \
                                                         __func__, __LINE__)

IRAM_ATTR bool
nc::ir_interface::rmt_rx_done_callback(rmt_channel_handle_t rx_chan,
                                       const rmt_rx_done_event_data_t *edata,
                                       void *user_data)
{
    // This runs in an ISR context - keep it minimal
    nc::ir_interface *ir_interface = static_cast<nc::ir_interface *>(user_data);
    return ir_interface->_handle_rx_done(edata);
}

void nc::ir_interface::packet_processing_task(void *arg)
{
    nc::ir_interface *ir_interface = static_cast<nc::ir_interface *>(arg);
    nc::message::ir_packet packet;

    while (true) {
        // Wait for packets from the queue
        if (xQueueReceive(ir_interface->_packet_queue, &packet,
                          portMAX_DELAY) == pdTRUE) {
            // Process the packet if we have a handler
            if (ir_interface->_packet_handler) {
                ir_interface->_packet_handler(&packet);
            }
        }
    }
}

nc::ir_interface::ir_interface(gpio_num_t tx_gpio_num, gpio_num_t rx_gpio_num,
                               uint32_t resolution_hz, uint32_t frequency_hz)
    : _logger("IR Interface", nsec::config::logging::ir_interface_level),
      _packet_queue(nullptr), _packet_task(nullptr)
{
    _logger.info("Setting up IR communication");

    // Configure RMT TX channel
    rmt_tx_channel_config_t tx_config = {
        .gpio_num = tx_gpio_num,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = resolution_hz,
        .mem_block_symbols = 48,
        .trans_queue_depth = 4,
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_config, &_ir_tx_channel));

    // Configure RMT RX channel
    rmt_rx_channel_config_t rx_config = {
        .gpio_num = rx_gpio_num,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = resolution_hz,
        .mem_block_symbols = 48,
    };
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_config, &_ir_rx_channel));

    // Set up IR carrier (38kHz)
    rmt_carrier_config_t carrier_config = {
        .frequency_hz = frequency_hz,
        .duty_cycle = 0.33f,
    };
    ESP_ERROR_CHECK(rmt_apply_carrier(_ir_tx_channel, &carrier_config));

    // Create IR encoder (NEC protocol is commonly used for IR)
    rmt_bytes_encoder_config_t encoder_config = {
        .bit0 =
            {
                .duration0 = 560, // NEC protocol timing in microseconds
                .level0 = 1,
                .duration1 = 560,
                .level1 = 0,
            },
        .bit1 =
            {
                .duration0 = 560,
                .level0 = 1,
                .duration1 = 1690,
                .level1 = 0,
            },
        .flags = {.msb_first = true},
    };
    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&encoder_config, &_ir_encoder));

    // Register RX callback using static IRAM_ATTR method
    rmt_rx_event_callbacks_t cbs = {.on_recv_done = rmt_rx_done_callback};
    ESP_ERROR_CHECK(
        rmt_rx_register_event_callbacks(_ir_rx_channel, &cbs, this));

    // Enable channels
    ESP_ERROR_CHECK(rmt_enable(_ir_tx_channel));
    ESP_ERROR_CHECK(rmt_enable(_ir_rx_channel));

    // Create queue for packet processing
    _packet_queue = xQueueCreate(5, sizeof(nc::message::ir_packet));
    if (_packet_queue == nullptr) {
        _logger.error("Failed to create packet queue");
        NSEC_THROW_COMMUNICATION_ERROR("Failed to create packet queue");
    }

    // Create task to process packets outside ISR context
    BaseType_t task_created = xTaskCreate(
        packet_processing_task, "ir_packet_task", 2048, this, 5, &_packet_task);

    if (task_created != pdPASS) {
        _logger.error("Failed to create packet processing task");
        NSEC_THROW_COMMUNICATION_ERROR(
            "Failed to create packet processing task");
    }

    _logger.info("IR communication setup complete");
}

nc::ir_interface::~ir_interface()
{
    // Clean up resources
    if (_packet_task != nullptr) {
        vTaskDelete(_packet_task);
    }

    if (_packet_queue != nullptr) {
        vQueueDelete(_packet_queue);
    }
}

size_t nc::ir_interface::send(const uint8_t *payload, size_t payload_size)
{
    _logger.debug("Sending IR data: {} bytes", payload_size);

    rmt_transmit_config_t tx_config = {
        .loop_count = 0, // No looping
    };

    esp_err_t err = rmt_transmit(_ir_tx_channel, _ir_encoder, payload,
                                 payload_size, &tx_config);

    if (err != ESP_OK) {
        _logger.error("Failed to transmit IR data: {}", esp_err_to_name(err));
        NSEC_THROW_COMMUNICATION_ERROR("IR transmission failed");
    }

    return payload_size;
}

void nc::ir_interface::register_packet_handler(ir_packet_handler_t handler)
{
    _packet_handler = std::move(handler);
    _logger.debug("Packet handler registered");
}

void nc::ir_interface::receive(uint8_t *buffer, size_t buffer_size)
{
    _logger.debug("Setting up IR reception, buffer size: {}", buffer_size);

    // Configure reception parameters
    rmt_receive_config_t rx_config = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 10000 * 1000, // 10 ms
    };

    // Use smart pointer to ensure cleanup in case of exceptions
    std::unique_ptr<rmt_symbol_word_t[]> symbols(new rmt_symbol_word_t[64]);

    esp_err_t err = rmt_receive(_ir_rx_channel, symbols.get(),
                                sizeof(rmt_symbol_word_t) * 64, &rx_config);

    if (err != ESP_OK) {
        _logger.error("Failed to start IR reception: {}", esp_err_to_name(err));
        NSEC_THROW_COMMUNICATION_ERROR("IR reception setup failed");
    }

    // Transfer ownership to the RMT driver only if successful
    symbols.release();

    _logger.debug("IR reception successfully initiated");
    // Note: The actual data will be received through the callback registered in
    // constructor This method just initiates the receive operation
}

IRAM_ATTR bool
nc::ir_interface::_decode_rmt_symbols(const rmt_symbol_word_t *symbols,
                                      size_t symbol_count, uint8_t *data,
                                      size_t *data_len, size_t max_len)
{
    if (!symbols || !data || !data_len || symbol_count < 4) {
        // Avoid logging in ISR context
        return false;
    }

    // Clear output buffer
    memset(data, 0, max_len);

    size_t byte_index = 0;
    uint8_t bit_index = 0;

    // Validate header - typical NEC protocol has a long header pulse
    // No logging in ISR context, just continue processing

    // Start from after header (usually first 2 symbols are header)
    size_t i = 2;

    while (i < symbol_count && byte_index < max_len) {
        // We expect alternating high/low signals for each bit
        if (symbols[i].level0 == 1 && symbols[i].level1 == 0) {
            // The high period is typically consistent, low period determines
            // bit value Longer low period (>1000µs) typically means '1',
            // shorter means '0'
            if (symbols[i].duration1 > 1000) {
                data[byte_index] |= (1 << bit_index);
            }

            bit_index++;
            if (bit_index >= 8) {
                bit_index = 0;
                byte_index++;
                if (byte_index < max_len) {
                    data[byte_index] = 0; // Initialize next byte
                }
            }
        }
        i++;
    }

    // Include partial byte if we have one
    *data_len = byte_index + (bit_index > 0 ? 1 : 0);

    return byte_index > 0 ||
           bit_index > 0; // Return true if we decoded anything
}

IRAM_ATTR bool
nc::ir_interface::_handle_rx_done(const rmt_rx_done_event_data_t *edata)
{
    // This runs in ISR context - we must be minimal and IRAM-safe
    // No memory allocations, logging, or complex operations
    uint8_t decoded_data[32];
    std::size_t decoded_len = 0;

    if (_decode_rmt_symbols(edata->received_symbols, edata->num_symbols,
                            decoded_data, &decoded_len, 32)) {
        if (decoded_len >= sizeof(nc::message::ir_packet)) {
            nc::message::ir_packet *packet =
                reinterpret_cast<nc::message::ir_packet *>(decoded_data);

            uint8_t calculated_checksum = 0;
            // Calculate checksum (XOR of all bytes except checksum field)
            for (size_t i = 0;
                 i < sizeof(nc::message::ir_packet) - sizeof(uint16_t); i++) {
                calculated_checksum ^= decoded_data[i];
            }

            uint16_t packet_checksum = packet->checksum;

            if (calculated_checksum == packet_checksum) {
                // Send packet to queue for processing outside ISR context
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;

                // Using xQueueSendFromISR since we're in an ISR context
                xQueueSendFromISR(_packet_queue, packet,
                                  &xHigherPriorityTaskWoken);

                // Yield if we woke a higher priority task
                if (xHigherPriorityTaskWoken == pdTRUE) {
                    portYIELD_FROM_ISR();
                }
            }
        }
    }

    rmt_receive_config_t rx_config = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 10000 * 1000, // 10 ms
    };

    // Static buffer for RMT symbols (safer for ISR context)
    static rmt_symbol_word_t new_symbols[64];
    rmt_receive(_ir_rx_channel, new_symbols, sizeof(rmt_symbol_word_t) * 64,
                &rx_config);

    return false; // Return false to prevent higher level interrupt handling
}
