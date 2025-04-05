/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 * Copyright 2025 Abdelhakim Qbaich <abdelhakim@qbaich.com>
 */

#ifndef NSEC_COMMUNICATION_IR_INTERFACE_HPP
#define NSEC_COMMUNICATION_IR_INTERFACE_HPP

#include "driver/rmt_encoder.h"
#include "driver/rmt_rx.h"
#include "driver/rmt_tx.h"

#include <cstdint>
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
    communication_error(const string &msg, const char *file_name,
                        const char *function_name, unsigned int line_number);
};
class protocol_error : public communication_error
{
  public:
    protocol_error(const string &msg, const char *file_name,
                   const char *function_name, unsigned int line_number);
};
} // namespace exception

class ir_interface
{
  public:
    ir_interface(gpio_num_t tx_gpio_num, gpio_num_t rx_gpio_num,
                 uint32_t resolution_hz, uint32_t frequency_hz);

    size_t send(const uint8_t *payload, size_t payload_size);
    void receive(uint8_t *buffer, size_t buffer_size);

  private:
    rmt_channel_handle_t _ir_tx_channel;
    rmt_channel_handle_t _ir_rx_channel;
    rmt_encoder_handle_t _ir_encoder;
    nsec::logging::logger _logger;
};

} // namespace nsec::communication

#endif // NSEC_COMMUNICATION_IR_INTERFACE_HPP
