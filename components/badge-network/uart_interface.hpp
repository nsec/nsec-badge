/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_COMMUNICATION_UART_INTERFACE_HPP
#define NSEC_COMMUNICATION_UART_INTERFACE_HPP

#include "driver/uart.h"

#include <cstdint>
#include <optional>
#include <string>

#include <utils/exception.hpp>
#include <utils/logging.hpp>

namespace nsec::communication
{

namespace exception
{
class communication_error : public nsec::exception::runtime_error
{
  public:
    communication_error(const std::string &msg, const char *file_name,
                        const char *function_name, unsigned int line_number);
};
class protocol_error : public communication_error
{
  public:
    protocol_error(const std::string &msg, const char *file_name,
                   const char *function_name, unsigned int line_number);
};
} // namespace exception

class uart_interface
{
  public:
    uart_interface(uart_port_t hw_uart_id, unsigned int rx_pin_id,
                   unsigned int tx_pin_id, unsigned int baud_rate,
                   bool inverted_logic);

    std::size_t send(std::uint8_t data_byte);
    std::size_t send(const std::uint8_t *data_bytes, std::size_t length);
    std::size_t available_data_size();
    std::uint8_t peek();
    std::uint8_t receive();
    void receive(std::uint8_t *out_payload, std::size_t length);
    void reset();

  private:
    void _drain_queue();

    QueueHandle_t _queue;
    uart_port_t _hw_uart_id;
    nsec::logging::logger _logger;
    std::size_t _available_data_size;
    std::optional<std::uint8_t> _peek_byte;
};

} // namespace nsec::communication

#endif // NSEC_COMMUNICATION_UART_INTERFACE_HPP
