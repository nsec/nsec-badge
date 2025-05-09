/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2025 Abdelhakim Qbaich <abdelhakim@qbaich.com>
 */

#ifndef NSEC_COMMUNICATION_IR_INTERFACE_HPP
#define NSEC_COMMUNICATION_IR_INTERFACE_HPP

#include <cstdint>
#include <functional>

#include "driver/gpio.h"
#include "driver/rmt_rx.h"
#include "driver/rmt_tx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <utils/config.hpp>
#include <utils/exception.hpp>
#include <utils/logging.hpp>

#include "network_messages.hpp"

#define NEC_DECODE_MARGIN_US 300 // Tolerance for decoding pulses (microseconds)

#define NEC_LEADING_HIGH_US 9000
#define NEC_LEADING_LOW_US 4500
#define NEC_DATA_HIGH_US 560
#define NEC_DATA_ZERO_LOW_US 560
#define NEC_DATA_ONE_LOW_US 1690
#define NEC_ENDING_HIGH_US 4000

namespace nsec::communication
{

namespace exception
{

class protocol_error : public nsec::exception::runtime_error
{
  public:
    protocol_error(const std::string &message, const char *file,
                   const char *func, int line)
        : nsec::exception::runtime_error(message, file, func, line)
    {
    }
};

} // namespace exception

using rmt_ir_packet_encoder_t = struct {
    rmt_encoder_t base;
    rmt_encoder_t *copy_encoder; // To encode the leading and ending pulse
    rmt_encoder_t *bytes_encoder;
    rmt_symbol_word_t leading_symbol;
    rmt_symbol_word_t ending_symbol;
    int state;
};

class ir_interface
{
  public:
    // Callback function type for handling received packets
    using ir_packet_handler_t =
        std::function<void(const message::ir_packet *packet)>;

    ir_interface(gpio_num_t tx_gpio_num, gpio_num_t rx_gpio_num,
                 uint32_t resolution_hz, uint32_t frequency_hz,
                 size_t mem_block_symbols);
    ~ir_interface();

    ir_interface(const ir_interface &) = delete;
    ir_interface &operator=(const ir_interface &) = delete;
    ir_interface(ir_interface &&) = delete;
    ir_interface &operator=(ir_interface &&) = delete;

    bool send(const uint8_t *payload, size_t payload_size);

    void register_packet_handler(ir_packet_handler_t handler);

  private:
    std::vector<rmt_symbol_word_t> _rx_symbols;

    nsec::logging::logger _logger;

    rmt_channel_handle_t _tx_channel = nullptr;
    rmt_channel_handle_t _rx_channel = nullptr;
    rmt_encoder_handle_t _custom_encoder = nullptr;

    QueueHandle_t _receive_queue = nullptr;
    TaskHandle_t _reception_task_handle = nullptr;
    ir_packet_handler_t _packet_handler = nullptr;

    void _initialize_reception_task();

    static void _reception_task_entry(void *arg);
    void _reception_task_impl();

    IRAM_ATTR bool
    _decode_rmt_symbols_nec(const rmt_symbol_word_t *symbols,
                            size_t symbol_count, std::vector<uint8_t> &data);
};

} // namespace nsec::communication

#endif // NSEC_COMMUNICATION_IR_INTERFACE_HPP
