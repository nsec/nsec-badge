/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include "uart_interface.hpp"
#include "utils/config.hpp"

#include <exception>

#include <fmt/format.h>

namespace nc = nsec::communication;

#define NSEC_THROW_COMMUNICATION_ERROR(msg)                                    \
    throw nsec::communication::exception::communication_error(                 \
        msg, __FILE__, __func__, __LINE__)
#define NSEC_THROW_PROTOCOL_ERROR(msg)                                         \
    throw nsec::communication::exception::protocol_error(msg, __FILE__,        \
                                                         __func__, __LINE__)

nc::exception::communication_error::communication_error(
    const std::string &msg, const char *file_name, const char *function_name,
    unsigned int line_number)
    : nsec::exception::runtime_error(msg, file_name, function_name, line_number)
{
}

nc::exception::protocol_error::protocol_error(const std::string &msg,
                                              const char *file_name,
                                              const char *function_name,
                                              unsigned int line_number)
    : nc::exception::communication_error(msg, file_name, function_name,
                                         line_number)
{
}

nc::uart_interface::uart_interface(uart_port_t hw_uart_id,
                                   unsigned int rx_pin_id,
                                   unsigned int tx_pin_id,
                                   unsigned int baud_rate, bool inverted_logic)
    : _hw_uart_id(hw_uart_id),
      _logger("uart_interface", nsec::config::logging::uart_interface_level),
      _available_data_size(0)
{
    // Match the defaults of last year's badge (AVR SoftwareSerial).
    const uart_config_t config = {
        // Apparently accepts negative baud rates, probably for time travelers.
        .baud_rate = int(baud_rate),
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        // Source clock is undocumented... assuming DEFAULT makes sense.
        .source_clk = UART_SCLK_DEFAULT,
    };

    // Configure UART parameters
    const auto set_config_ret = uart_param_config(hw_uart_id, &config);
    if (set_config_ret != ESP_OK) {
        NSEC_THROW_COMMUNICATION_ERROR(fmt::format(
            "Failed to set UART interface configuration: hw_uart_id={}, "
            "baud_rate={}",
            hw_uart_id, config.baud_rate));
    }

    const auto set_pin_ret =
        uart_set_pin(hw_uart_id, tx_pin_id, rx_pin_id, UART_PIN_NO_CHANGE,
                     UART_PIN_NO_CHANGE);
    if (set_pin_ret != ESP_OK) {
        NSEC_THROW_COMMUNICATION_ERROR(
            fmt::format("Failed to initialize UART interface: hw_uart_id={}, "
                        "rx_pin={}, tx_pin={}",
                        hw_uart_id, rx_pin_id, tx_pin_id));
    }

    const auto set_invert_ret = uart_set_line_inverse(
        hw_uart_id,
        inverted_logic ? (UART_SIGNAL_RXD_INV | UART_SIGNAL_TXD_INV) : 0);
    if (set_invert_ret != ESP_OK) {
        NSEC_THROW_COMMUNICATION_ERROR(
            fmt::format("Failed to set UART inversion mode: invert={}",
                        nsec::config::communication::invert_uart));
    }

    const auto driver_install_ret = uart_driver_install(
        hw_uart_id, nsec::config::communication::rx_buffer_size,
        nsec::config::communication::tx_buffer_size,
        nsec::config::communication::uart_queue_length, &_queue, 0);
    if (driver_install_ret != ESP_OK) {
        NSEC_THROW_COMMUNICATION_ERROR(
            fmt::format("Failed to install UART driver: hw_uart_id={}, "
                        "rx_buffer_size={}, tx_buffer_size={}, queue_size={}",
                        hw_uart_id, nsec::config::communication::rx_buffer_size,
                        nsec::config::communication::tx_buffer_size,
                        nsec::config::communication::uart_queue_length));
    }

    const auto set_mode_ret = uart_set_mode(hw_uart_id, UART_MODE_UART);
    if (set_mode_ret) {
        NSEC_THROW_COMMUNICATION_ERROR(
            "Failed to set UART mode to regular UART mode");
    }
}

std::size_t nc::uart_interface::send(std::uint8_t data_byte)
{
    return send(&data_byte, 1);
}

std::size_t nc::uart_interface::send(const std::uint8_t *data_bytes,
                                     std::size_t length)
{
    if (length == 0) {
        return 0;
    }

    _logger.debug("Sending data: size={}", length);

    const auto write_ret = uart_write_bytes(_hw_uart_id, data_bytes, length);
    if (write_ret < 0) {
        NSEC_THROW_COMMUNICATION_ERROR(fmt::format(
            "Failed to write bytes to UART interface: buffer={}, size={}",
            static_cast<const void *>(data_bytes), length));
    }

    return write_ret;
}

std::size_t nc::uart_interface::available_data_size()
{
    _drain_queue();
    return _available_data_size + (_peek_byte.has_value() ? 1 : 0);
}

std::uint8_t nc::uart_interface::peek()
{
    if (_available_data_size == 0) {
        NSEC_THROW_PROTOCOL_ERROR(fmt::format(
            "Attempted peek when no data is available: hw_uart_id={}",
            _hw_uart_id));
    }

    if (!_peek_byte) {
        std::uint8_t first_byte = 0;
        receive(&first_byte, sizeof(first_byte));

        _peek_byte = first_byte;
    }

    return *_peek_byte;
}

void nc::uart_interface::receive(std::uint8_t *out_payload, std::size_t length)
{
    if (available_data_size() < length) {
        NSEC_THROW_PROTOCOL_ERROR(
            fmt::format("Attempted to read more data than is available: "
                        "hw_uart_id={}, requested_size={}, available_size={}",
                        _hw_uart_id, length, available_data_size()));
    }

    _logger.debug("Consuming data: size={}", length);
    _drain_queue();

    if (_peek_byte && length > 0) {
        *out_payload = *_peek_byte;
        length--;
        out_payload++;
        _peek_byte.reset();
    }

    if (length == 0) {
        return;
    }

    const auto read_ret = uart_read_bytes(_hw_uart_id, out_payload, length, 0);
    if (read_ret != length) {
        NSEC_THROW_COMMUNICATION_ERROR(fmt::format(
            "Failed to receive data payload that was expected to be available: "
            "hw_uart_id={}, requested_size={}, available_size={}",
            _hw_uart_id, length, _available_data_size));
    }

    _available_data_size -= length;
}

std::uint8_t nc::uart_interface::receive()
{
    std::uint8_t the_byte;
    receive(&the_byte, sizeof(the_byte));
    return the_byte;
}

void nc::uart_interface::reset()
{
    const auto reset_input_ret = uart_flush_input(_hw_uart_id);
    if (reset_input_ret != ESP_OK) {
        NSEC_THROW_COMMUNICATION_ERROR(fmt::format(
            "Failed to reset UART input: hw_uart_id={}", _hw_uart_id));
    }

    // FreeRTOS docs say we can assume this is successful.
    xQueueReset(_queue);
}

void nc::uart_interface::_drain_queue()
{
    _logger.debug("Draining event queue");

    uart_event_t event;
    while (xQueueReceive(_queue, &event, 0)) {
        switch (event.type) {
        case UART_DATA:
            // UART data event
            _available_data_size += event.size;
            _logger.debug(
                "Data received event: size={}, available_data_size={}",
                event.size, _available_data_size);
            break;
        case UART_BREAK:
            // UART break event
            _logger.debug("Received \"break\" event");
            break;
        case UART_BUFFER_FULL:
            // UART RX buffer full event
            NSEC_THROW_COMMUNICATION_ERROR(fmt::format(
                "Unexpected UART buffer full event received: hw_uart_id={}",
                _hw_uart_id));
        case UART_FIFO_OVF:
            // UART FIFO overflow event
            NSEC_THROW_COMMUNICATION_ERROR(fmt::format(
                "Unexpected UART FIFO overflow event received: hw_uart_id={}",
                _hw_uart_id));
        case UART_FRAME_ERR:
            // UART RX frame error event
            NSEC_THROW_COMMUNICATION_ERROR(fmt::format(
                "Unexpected UART frame error event received: hw_uart_id={}",
                _hw_uart_id));
        case UART_PARITY_ERR:
            // UART RX parity event
            NSEC_THROW_COMMUNICATION_ERROR(fmt::format(
                "Unexpected UART parity error event received: hw_uart_id={}",
                _hw_uart_id));
        case UART_DATA_BREAK:
            // UART TX data and break event
            NSEC_THROW_COMMUNICATION_ERROR(
                fmt::format("Unexpected UART TX data and break event received: "
                            "hw_uart_id={}",
                            _hw_uart_id));
        case UART_PATTERN_DET:
            // UART pattern detected
            NSEC_THROW_COMMUNICATION_ERROR(
                fmt::format("Unexpected UART pattern detected event received: "
                            "hw_uart_id={}",
                            _hw_uart_id));
        case UART_WAKEUP:
            // UART wakeup event
            NSEC_THROW_COMMUNICATION_ERROR(fmt::format(
                "Unexpected UART wake-up event received: hw_uart_id={}",
                _hw_uart_id));
        default:
            NSEC_THROW_COMMUNICATION_ERROR(
                fmt::format("Unexpected UART event received: hw_uart_id={}, "
                            "event_type_id={}",
                            _hw_uart_id, event.type));
        }
    }
}