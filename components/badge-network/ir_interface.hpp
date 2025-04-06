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
#include <functional>
#include <string>

#include <badge/id.hpp>
#include <utils/exception.hpp>
#include <utils/logging.hpp>

#include "network_messages.hpp"
#include <utils/logging.hpp>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "network_messages.hpp"

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

class ir_interface
{
  public:
  ir_interface(gpio_num_t tx_gpio_num, gpio_num_t rx_gpio_num,
               uint32_t resolution_hz, uint32_t frequency_hz);
  ~ir_interface();

    size_t send(const uint8_t *payload, size_t payload_size);
    void receive(uint8_t *buffer, size_t buffer_size);

    using ir_packet_handler_t = std::function<void(const message::ir_packet *)>;
    void register_packet_handler(ir_packet_handler_t handler);


private:
    rmt_channel_handle_t _ir_tx_channel;
    rmt_channel_handle_t _ir_rx_channel;
    rmt_encoder_handle_t _ir_encoder;

    nsec::logging::logger _logger;
    ir_packet_handler_t _packet_handler;

    // Queue for processing packets outside of ISR context
    QueueHandle_t _packet_queue;
    TaskHandle_t _packet_task;

    static void packet_processing_task(void* arg);

    IRAM_ATTR bool _handle_rx_done(const rmt_rx_done_event_data_t *edata);
    IRAM_ATTR bool _decode_rmt_symbols(const rmt_symbol_word_t *symbols,
                                      size_t symbol_count, uint8_t *data,
                                      size_t *data_len, size_t max_len);
    static IRAM_ATTR bool rmt_rx_done_callback(rmt_channel_handle_t rx_chan,
                                               const rmt_rx_done_event_data_t *edata,
                                               void *user_data);
};

} // namespace nsec::communication

#endif // NSEC_COMMUNICATION_IR_INTERFACE_HPP
