/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2025 Abdelhakim Qbaich <abdelhakim@qbaich.com>
 */

#include "ir_interface.hpp"
#include "esp_attr.h"

#include <esp_log.h>

namespace nc = nsec::communication;

#define NSEC_THROW_PROTOCOL_ERROR(msg)                                         \
    throw nc::exception::protocol_error(msg, __FILE__, __func__, __LINE__)

static size_t rmt_encode_custom_ir(rmt_encoder_t *encoder,
                                   rmt_channel_handle_t channel,
                                   const void *data, size_t data_size,
                                   rmt_encode_state_t *ret_state)
{
    size_t encoded_symbols = 0;

    rmt_encode_state_t current_state = RMT_ENCODING_RESET;
    rmt_encode_state_t session_state = RMT_ENCODING_RESET;

    nc::rmt_custom_ir_encoder_t *custom_encoder =
        __containerof(encoder, nc::rmt_custom_ir_encoder_t, base);
    rmt_encoder_handle_t copy_encoder = custom_encoder->copy_encoder;
    rmt_encoder_handle_t bytes_encoder = custom_encoder->bytes_encoder;

    switch (custom_encoder->state) {
    case 0: // Send leader code
        encoded_symbols += copy_encoder->encode(
            copy_encoder, channel, &custom_encoder->nec_leading_symbol,
            sizeof(rmt_symbol_word_t), &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            custom_encoder->state = 1;
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            current_state = static_cast<rmt_encode_state_t>(
                current_state | RMT_ENCODING_MEM_FULL);
            goto out;
        }
        [[fallthrough]];

    case 1: // Send data
        encoded_symbols += bytes_encoder->encode(
            bytes_encoder, channel, static_cast<const uint8_t *>(data),
            data_size, &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            custom_encoder->state = 2;
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            current_state = static_cast<rmt_encode_state_t>(
                current_state | RMT_ENCODING_MEM_FULL);
            goto out;
        }
        [[fallthrough]];

    case 2: // Send ending code
        encoded_symbols += copy_encoder->encode(
            copy_encoder, channel, &custom_encoder->nec_ending_symbol,
            sizeof(rmt_symbol_word_t), &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            custom_encoder->state = RMT_ENCODING_RESET;
            current_state = static_cast<rmt_encode_state_t>(
                current_state | RMT_ENCODING_COMPLETE);
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            current_state = static_cast<rmt_encode_state_t>(
                current_state | RMT_ENCODING_MEM_FULL);
        }
    }

out:
    *ret_state = current_state;
    return encoded_symbols;
}

static esp_err_t rmt_custom_ir_encoder_reset(rmt_encoder_t *encoder)
{
    nc::rmt_custom_ir_encoder_t *custom_encoder =
        __containerof(encoder, nc::rmt_custom_ir_encoder_t, base);
    rmt_encoder_reset(custom_encoder->copy_encoder);
    rmt_encoder_reset(custom_encoder->bytes_encoder);
    custom_encoder->state = RMT_ENCODING_RESET;
    return ESP_OK;
}

static esp_err_t rmt_del_custom_ir_encoder(rmt_encoder_t *encoder)
{
    nc::rmt_custom_ir_encoder_t *custom_encoder =
        __containerof(encoder, nc::rmt_custom_ir_encoder_t, base);
    rmt_del_encoder(custom_encoder->copy_encoder);
    rmt_del_encoder(custom_encoder->bytes_encoder);
    free(custom_encoder);
    return ESP_OK;
}

static esp_err_t rmt_new_custom_ir_encoder(uint32_t resolution_hz,
                                           rmt_encoder_handle_t *ret_encoder)
{
    auto calculate_ticks = [&](uint32_t duration_us) -> uint16_t {
        uint32_t ticks = ((uint64_t)duration_us * resolution_hz) / 1000000ULL;
        if (ticks > 0x7FFF) {
            throw std::overflow_error(
                "Overflowed on conversion from us to ticks");
        }
        return static_cast<uint16_t>(ticks);
    };

    if (!ret_encoder) {
        return ESP_ERR_INVALID_ARG;
    }

    nc::rmt_custom_ir_encoder_t *custom_encoder =
        static_cast<nc::rmt_custom_ir_encoder_t *>(
            rmt_alloc_encoder_mem(sizeof(nc::rmt_custom_ir_encoder_t)));
    if (!custom_encoder) {
        return ESP_ERR_NO_MEM;
    }

    custom_encoder->base.encode = rmt_encode_custom_ir;
    custom_encoder->base.reset = rmt_custom_ir_encoder_reset;
    custom_encoder->base.del = rmt_del_custom_ir_encoder;
    custom_encoder->state = RMT_ENCODING_RESET;

    rmt_copy_encoder_config_t copy_encoder_config = {};
    rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 =
            {
                .duration0 = calculate_ticks(NEC_DATA_HIGH_US),
                .level0 = 1,
                .duration1 = calculate_ticks(NEC_DATA_ZERO_LOW_US),
                .level1 = 0,
            },
        .bit1 =
            {
                .duration0 = calculate_ticks(NEC_DATA_HIGH_US),
                .level0 = 1,
                .duration1 = calculate_ticks(NEC_DATA_ONE_LOW_US),
                .level1 = 0,
            },
    };

    esp_err_t ret = rmt_new_copy_encoder(&copy_encoder_config,
                                         &custom_encoder->copy_encoder);
    if (ret != ESP_OK) {
        ESP_LOGE("IR Encoder", "Failed to create copy encoder: %s",
                 esp_err_to_name(ret));
        goto err;
    }
    ret = rmt_new_bytes_encoder(&bytes_encoder_config,
                                &custom_encoder->bytes_encoder);
    if (ret != ESP_OK) {
        ESP_LOGE("IR Encoder", "Failed to create bytes encoder: %s",
                 esp_err_to_name(ret));
        goto err;
    }

    custom_encoder->nec_leading_symbol = (rmt_symbol_word_t){
        .duration0 = calculate_ticks(NEC_LEADING_HIGH_US),
        .level0 = 1,
        .duration1 = calculate_ticks(NEC_LEADING_LOW_US),
        .level1 = 0,
    };
    custom_encoder->nec_ending_symbol = (rmt_symbol_word_t){
        .duration0 = calculate_ticks(NEC_ENDING_HIGH_US),
        .level0 = 1,
        .duration1 = 0x7FFF,
        .level1 = 0,
    };

    *ret_encoder = &custom_encoder->base;
    return ESP_OK;

err:
    if (custom_encoder->bytes_encoder) {
        rmt_del_encoder(custom_encoder->bytes_encoder);
    }
    if (custom_encoder->copy_encoder) {
        rmt_del_encoder(custom_encoder->copy_encoder);
    }
    free(custom_encoder);
    return ret;
}

static IRAM_ATTR bool
rmt_rx_done_callback(rmt_channel_handle_t channel,
                     const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = static_cast<QueueHandle_t>(user_data);
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

nc::ir_interface::ir_interface(gpio_num_t tx_gpio_num, gpio_num_t rx_gpio_num,
                               uint32_t resolution_hz, uint32_t frequency_hz,
                               size_t mem_block_symbols)
    : _rx_symbols(mem_block_symbols * 4), // FIXME
      _logger("IR Interface", nsec::config::logging::ir_interface_level)
{
    _logger.debug("Setting up IR communication (TX: {}, RX: {})", tx_gpio_num,
                  rx_gpio_num);

    /* RMT TX Configuration */
    rmt_tx_channel_config_t tx_config = {
        .gpio_num = tx_gpio_num,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = resolution_hz,
        .mem_block_symbols = mem_block_symbols,
        .trans_queue_depth = 4,
    };
    esp_err_t err = rmt_new_tx_channel(&tx_config, &_tx_channel);
    if (err != ESP_OK) {
        NSEC_THROW_PROTOCOL_ERROR(fmt::format("Failed to create TX channel: {}",
                                              esp_err_to_name(err)));
    }

    rmt_carrier_config_t carrier_config = {
        .frequency_hz = frequency_hz,
        .duty_cycle = 0.33,
    };
    err = rmt_apply_carrier(_tx_channel, &carrier_config);
    if (err != ESP_OK) {
        NSEC_THROW_PROTOCOL_ERROR(fmt::format(
            "Failed to apply carrier to TX channel: {}", esp_err_to_name(err)));
    }

    /* RMT RX Configuration */
    rmt_rx_channel_config_t rx_config = {
        .gpio_num = rx_gpio_num,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = resolution_hz,
        .mem_block_symbols = mem_block_symbols,
    };
    err = rmt_new_rx_channel(&rx_config, &_rx_channel);
    if (err != ESP_OK) {
        NSEC_THROW_PROTOCOL_ERROR(fmt::format(
            "Failed to create RMT RX channel: {}", esp_err_to_name(err)));
    }

    _receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
    if (_receive_queue == nullptr) {
        NSEC_THROW_PROTOCOL_ERROR("Failed to create RX queue");
    }
    rmt_rx_event_callbacks_t cbs = {.on_recv_done = rmt_rx_done_callback};
    err = rmt_rx_register_event_callbacks(_rx_channel, &cbs, _receive_queue);
    if (err != ESP_OK) {
        NSEC_THROW_PROTOCOL_ERROR(fmt::format(
            "Failed to register RX callbacks: {}", esp_err_to_name(err)));
    }

    err = rmt_new_custom_ir_encoder(resolution_hz, &_custom_encoder);
    if (err != ESP_OK) {
        NSEC_THROW_PROTOCOL_ERROR(fmt::format(
            "Failed to create custom IR encoder: {}", esp_err_to_name(err)));
    }

    err = rmt_enable(_tx_channel);
    if (err != ESP_OK) {
        NSEC_THROW_PROTOCOL_ERROR(fmt::format("Failed to enable TX channel: {}",
                                              esp_err_to_name(err)));
    }
    err = rmt_enable(_rx_channel);
    if (err != ESP_OK) {
        NSEC_THROW_PROTOCOL_ERROR(fmt::format("Failed to enable RX channel: {}",
                                              esp_err_to_name(err)));
    }

    _initialize_reception_task();

    _logger.info("IR interface setup complete");
}

nc::ir_interface::~ir_interface()
{
    _logger.info("Cleaning up IR interface");

    if (_reception_task_handle != nullptr) {
        vTaskDelete(_reception_task_handle);
        _reception_task_handle = nullptr;
    }
    if (_receive_queue != nullptr) {
        vQueueDelete(_receive_queue);
        _receive_queue = nullptr;
    }

    if (_tx_channel) {
        rmt_disable(_tx_channel);
    }
    if (_rx_channel) {
        rmt_disable(_rx_channel);
    }

    if (_custom_encoder) {
        rmt_del_encoder(_custom_encoder);
        _custom_encoder = nullptr;
    }

    if (_tx_channel) {
        rmt_del_channel(_tx_channel);
        _tx_channel = nullptr;
    }
    if (_rx_channel) {
        rmt_del_channel(_rx_channel);
        _rx_channel = nullptr;
    }
}

void nc::ir_interface::_initialize_reception_task()
{
    BaseType_t task_created =
        xTaskCreate(_reception_task_entry, "ir_reception_task", 8192, this,
                    configMAX_PRIORITIES - 2, &_reception_task_handle);

    if (task_created != pdPASS) {
        if (_receive_queue != nullptr) {
            vQueueDelete(_receive_queue);
            _receive_queue = nullptr;
        }
        NSEC_THROW_PROTOCOL_ERROR("Failed to create IR reception task");
    }

    _logger.debug("IR reception task created successfully");
}

bool nc::ir_interface::send(const uint8_t *payload, size_t payload_size)
{
    if (payload == nullptr || payload_size == 0) {
        _logger.warn("Attempted to send empty or null IR payload");
        return false;
    }
    if (!_tx_channel || !_custom_encoder) {
        _logger.error("Cannot send, TX channel or encoder not initialized");
        return false;
    }

    _logger.debug("Sending IR data: {} bytes", payload_size);

    rmt_encoder_reset(_custom_encoder);

    rmt_transmit_config_t tx_config = {
        .loop_count = 0, // No loop
    };

    esp_err_t err = rmt_transmit(_tx_channel, _custom_encoder, payload,
                                 payload_size, &tx_config);
    if (err != ESP_OK) {
        _logger.error("Failed to transmit IR data: {}", esp_err_to_name(err));
        return false;
    }

    // FIXME Should have a cfg value for timeout
    err = rmt_tx_wait_all_done(_tx_channel, pdMS_TO_TICKS(5000));
    if (err == ESP_ERR_TIMEOUT) {
        _logger.warn("Timeout waiting for IR transmission completion");
        return false;
    } else if (err != ESP_OK) {
        _logger.error("Error waiting for IR transmission completion: {}",
                      esp_err_to_name(err));
        return false;
    }

    _logger.debug("IR transmission complete");
    return true;
}

void nc::ir_interface::register_packet_handler(ir_packet_handler_t handler)
{
    _packet_handler = std::move(handler);
    _logger.info("IR packet handler registered");
}

void nc::ir_interface::_reception_task_entry(void *arg)
{
    nc::ir_interface *instance = static_cast<nc::ir_interface *>(arg);
    instance->_reception_task_impl();
}

void nc::ir_interface::_reception_task_impl()
{
    rmt_rx_done_event_data_t rx_data;
    // FIXME Should have constants for these
    rmt_receive_config_t rx_config = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 12500000,
    };

    std::vector<uint8_t> decoded_data_buffer;
    decoded_data_buffer.reserve(sizeof(nc::message::ir_packet) * 2); // FIXME

    _logger.debug("Reception task started, entering main loop.");

    while (true) {
        if (!(_rx_channel && _receive_queue)) {
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }

        esp_err_t err = rmt_receive(
            _rx_channel, _rx_symbols.data(),
            _rx_symbols.size() * sizeof(rmt_symbol_word_t), &rx_config);
        if (err != ESP_OK) {
            _logger.error("Failed to start RMT receive: {}",
                          esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        if (xQueueReceive(_receive_queue, &rx_data, portMAX_DELAY) == pdTRUE) {
            _logger.debug("Received RMT symbols: {}", rx_data.num_symbols);

            decoded_data_buffer.clear();

            if (_decode_rmt_symbols_nec(rx_data.received_symbols,
                                        rx_data.num_symbols,
                                        decoded_data_buffer)) {
                _logger.debug("Successfully decoded {} bytes",
                              decoded_data_buffer.size());

                if (decoded_data_buffer.size() ==
                    sizeof(nc::message::ir_packet)) {
                    nc::message::ir_packet local_packet;
                    std::memcpy(&local_packet, decoded_data_buffer.data(),
                                sizeof(nc::message::ir_packet));

                    if (_packet_handler) {
                        _logger.debug("Calling packet handler");
                        _packet_handler(&local_packet);
                    } else {
                        _logger.warn(
                            "Received packet but no handler registered");
                    }
                } else if (!decoded_data_buffer.empty()) {
                    _logger.warn("Decoded length mismatch (Exp: {}, "
                                 "Got: {}). Packet ignored.",
                                 sizeof(nc::message::ir_packet),
                                 decoded_data_buffer.size());
                }
            } else {
                _logger.debug("Failed to decode RMT symbols");
            }
        }
    }
}

static inline bool nec_check_in_range(uint32_t signal_duration,
                                      uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + NEC_DECODE_MARGIN_US)) &&
           (signal_duration > (spec_duration - NEC_DECODE_MARGIN_US));
}

bool nc::ir_interface::_decode_rmt_symbols_nec(const rmt_symbol_word_t *symbols,
                                               size_t symbol_count,
                                               std::vector<uint8_t> &data)
{
    data.clear();
    if (!symbols || symbol_count < 2) {
        return false;
    }

    size_t current_symbol_idx = 0;
    const rmt_symbol_word_t *current_symbol = &symbols[current_symbol_idx];
    _logger.debug("Symbol {}: level0: {}, duration0: {}, level1: {}, "
                  "duration1: {}",
                  current_symbol_idx, current_symbol->level0,
                  current_symbol->duration0, current_symbol->level1,
                  current_symbol->duration1);

    // Check leader code
    if (!(
            // current_symbol->level0 == 1 &&
            // FIXME tick to us conversion of durations?
            nec_check_in_range(current_symbol->duration0,
                               NEC_LEADING_HIGH_US) &&
            // current_symbol->level1 == 0 &&
            nec_check_in_range(current_symbol->duration1,
                               NEC_LEADING_LOW_US))) {
        _logger.debug("Failed to find leader code");
        return false;
    }
    current_symbol_idx++;

    size_t current_byte = 0;
    uint8_t bit_index = 0;

    while (current_symbol_idx < symbol_count) {
        current_symbol = &symbols[current_symbol_idx];
        _logger.debug("Symbol {}: level0: {}, duration0: {}, level1: {}, "
                      "duration1: {}",
                      current_symbol_idx, current_symbol->level0,
                      current_symbol->duration0, current_symbol->level1,
                      current_symbol->duration1);

        // Check stop bit
        if (
            // current_symbol->level0 == 1 &&
            nec_check_in_range(current_symbol->duration0, NEC_ENDING_HIGH_US)
            // nec_check_in_range(current_symbol->duration1, 0)
            // && current_symbol->level1 == 0
        ) {
            if (bit_index == 0) {
                return true;
            } else {
                _logger.debug("NEC stop bit found mid-byte (bit index {})",
                              bit_index);
                data.clear();
                return false;
            }
        }

        // Check Data Bit
        if (
            // current_symbol->level0 == 1 &&
            nec_check_in_range(current_symbol->duration0, NEC_DATA_HIGH_US)
            // && current_symbol->level1 == 0
        ) {
            bool is_one = nec_check_in_range(current_symbol->duration1,
                                             NEC_DATA_ONE_LOW_US);
            bool is_zero = nec_check_in_range(current_symbol->duration1,
                                              NEC_DATA_ZERO_LOW_US);

            if (is_one) {
                current_byte |= (1 << bit_index);
            } else if (!is_zero) {
                _logger.debug("NEC data bit low pulse invalid duration");
                data.clear();
                return false;
            }

            bit_index++;
            if (bit_index >= 8) {
                data.push_back(current_byte);
                bit_index = 0;
                current_byte = 0;
            }
        } else {
            data.clear();
            return false;
        }
        current_symbol_idx++;
    }

    data.clear();
    return false;
}
