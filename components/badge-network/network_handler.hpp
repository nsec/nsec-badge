/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2025 Abdelhakim Qbaich <abdelhakim@qbaich.com>
 */

#ifndef NSEC_NETWORK_HANDLER_HPP
#define NSEC_NETWORK_HANDLER_HPP

#include <atomic>
#include <cstdint>
#include <mutex>

#include <esp_timer.h>

#include <badge/id.hpp>
#include <utils/logging.hpp>

#include "ir_interface.hpp"
#include "network_messages.hpp"

namespace nsec::communication
{

namespace exception
{

class communication_error : public nsec::exception::runtime_error
{
  public:
    communication_error(const std::string &message, const char *file,
                        const char *func, int line)
        : nsec::exception::runtime_error(message, file, func, line)
    {
    }
};

} // namespace exception

/**
 * @brief Represents the states of the IR communication protocol.
 */
enum class ir_protocol_state : uint8_t {
    IDLE,
    WAITING_FOR_PEER,
    SENDER,
    RECEIVER,
    COMPLETED,
    TIMEOUT,
    CANCELLED,
};

class network_handler
{
  public:
    network_handler();
    ~network_handler();

    network_handler(const network_handler &) = delete;
    network_handler &operator=(const network_handler &) = delete;
    network_handler(network_handler &&) = delete;
    network_handler &operator=(network_handler &&) = delete;

    /**
     * @brief Gets the current state of the IR protocol state machine.
     * @return The current ir_protocol_state.
     */
    ir_protocol_state get_ir_protocol_state() const noexcept;

    /**
     * @brief Initiates the IR key exchange process.
     * Transitions the state machine from IDLE to WAITING_FOR_PEER.
     */
    void start_ir_key_exchange() noexcept;

  private:
    ir_interface _ir_interface;

    std::atomic<ir_protocol_state> _current_ir_state;
    nsec::runtime::badge_unique_id _peer_id; // ID of the peer once established
    int64_t _ir_timeout_timestamp_ms; // Timestamp for the next timeout check
    esp_timer_handle_t _ir_timeout_handle = nullptr;

    mutable std::mutex _state_mutex; // Protects access to state variables

    nsec::logging::logger _logger;

    void _initialize_timer();
    void _initialize_ir_packet_handler();

    void _reset_exchange_state() noexcept;
    void _set_ir_protocol_state(ir_protocol_state next_state) noexcept;
    void _handle_successful_sync() noexcept;

    void _send_ir_packet(message::ir_packet_type type) noexcept;
    void _handle_received_ir_packet(const message::ir_packet &packet) noexcept;

    void _check_ir_timeouts() noexcept;
    static void _timer_callback(void *arg);
};

} // namespace nsec::communication

#endif // NSEC_NETWORK_HANDLER_HPP
