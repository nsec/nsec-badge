/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2025 Abdelhakim Qbaich <abdelhakim@qbaich.com>
 */

#include "network_handler.hpp"

#include <esp_random.h>

#include <badge-persistence/badge_store.hpp>
#include <badge/globals.hpp>

namespace nr = nsec::runtime;
namespace nc = nsec::communication;
namespace np = nsec::persistence;
namespace cfg = nsec::config::communication;

#define NSEC_THROW_COMMUNICATION_ERROR(msg)                                    \
    throw nc::exception::communication_error(msg, __FILE__, __func__, __LINE__)

template <>
struct fmt::formatter<nc::ir_protocol_state>
    : fmt::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(nc::ir_protocol_state state, FormatContext &ctx) const
    {
        string_view name = "UNKNOWN";
        switch (state) {
        case nc::ir_protocol_state::IDLE:
            name = "IDLE";
            break;
        case nc::ir_protocol_state::WAITING_FOR_PEER:
            name = "WAITING_FOR_PEER";
            break;
        case nc::ir_protocol_state::SENDER:
            name = "SENDER";
            break;
        case nc::ir_protocol_state::RECEIVER:
            name = "RECEIVER";
            break;
        case nc::ir_protocol_state::COMPLETED:
            name = "COMPLETED";
            break;
        case nc::ir_protocol_state::TIMEOUT:
            name = "TIMEOUT";
            break;
        case nc::ir_protocol_state::CANCELLED:
            name = "CANCELLED";
            break;
        }
        return fmt::formatter<string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<nc::message::ir_packet_type>
    : fmt::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(nc::message::ir_packet_type type, FormatContext &ctx) const
    {
        string_view name = "UNKNOWN";
        switch (type) {
        case nc::message::ir_packet_type::SYNC_REQUEST:
            name = "SYNC_REQUEST";
            break;
        case nc::message::ir_packet_type::SYNC_ACCEPT:
            name = "SYNC_ACCEPT";
            break;
        case nc::message::ir_packet_type::ID_DATA:
            name = "ID_DATA";
            break;
        case nc::message::ir_packet_type::ACK:
            name = "ACK";
            break;
        case nc::message::ir_packet_type::CANCEL:
            name = "CANCEL";
            break;
        }
        return fmt::formatter<string_view>::format(name, ctx);
    }
};

namespace
{

int64_t get_current_time_ms()
{
    return esp_timer_get_time() / 1000;
}

int64_t get_timeout_duration_ms()
{
    return cfg::ir_timeout_ms;
}

int64_t get_idle_delay_ms()
{
    return cfg::ir_idle_delay_ms;
}

uint8_t calculate_checksum(const nc::message::ir_packet &packet)
{
    uint8_t checksum = 0;
    const uint8_t *packet_ptr = reinterpret_cast<const uint8_t *>(&packet);
    // Checksum covers all fields before the checksum field itself
    for (size_t i = 0; i < offsetof(nc::message::ir_packet, checksum); ++i) {
        checksum ^= packet_ptr[i];
    }
    return checksum;
}

} // namespace

nc::network_handler::network_handler()
    : _ir_interface(nsec::board::ir::tx_pin, nsec::board::ir::rx_pin,
                    cfg::ir_resolution_hz, cfg::ir_frequency_hz),
      _current_ir_state{ir_protocol_state::IDLE}, _peer_id{},
      _ir_timeout_timestamp_ms{0},
      _logger("Network handler", nsec::config::logging::network_handler_level)
{
    _initialize_timer();
    _initialize_ir_packet_handler();
    _logger.info("Network handler initialized");
}

nc::network_handler::~network_handler()
{
    if (_ir_timeout_handle) {
        esp_err_t err = esp_timer_stop(_ir_timeout_handle);
        if (err != ESP_OK) {
            _logger.error("Failed to stop IR timeout timer: {}",
                          esp_err_to_name(err));
        }

        err = esp_timer_delete(_ir_timeout_handle);
        if (err != ESP_OK) {
            _logger.error("Failed to delete IR timeout timer: {}",
                          esp_err_to_name(err));
        }
        _ir_timeout_handle = nullptr;
    }
    _logger.info("Network handler deinitialized");
}

nc::ir_protocol_state
nc::network_handler::get_ir_protocol_state() const noexcept
{
    return _current_ir_state.load(std::memory_order_relaxed);
}

void nc::network_handler::_initialize_timer()
{
    const esp_timer_create_args_t timer_args = {
        .callback = _timer_callback,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "ir_timeout_timer",
        .skip_unhandled_events = false,
    };

    esp_err_t err = esp_timer_create(&timer_args, &_ir_timeout_handle);
    if (err != ESP_OK) {
        NSEC_THROW_COMMUNICATION_ERROR(fmt::format(
            "Failed to create IR timeout timer: {}", esp_err_to_name(err)));
    }

    err = esp_timer_start_periodic(_ir_timeout_handle,
                                   cfg::ir_timeout_check_interval_us);
    if (err != ESP_OK) {
        esp_timer_delete(_ir_timeout_handle);
        _ir_timeout_handle = nullptr;
        NSEC_THROW_COMMUNICATION_ERROR(fmt::format(
            "Failed to start IR timeout timer: {}", esp_err_to_name(err)));
    }
    _logger.debug("IR timeout timer started");
}

void nc::network_handler::_initialize_ir_packet_handler()
{
    _ir_interface.register_packet_handler(
        [this](const message::ir_packet *packet) {
            if (packet == nullptr) {
                _logger.error("Received null IR packet pointer");
                return;
            }
            if (calculate_checksum(*packet) != packet->checksum) {
                _logger.warn("Received IR packet with invalid checksum");
                return;
            }
            _handle_received_ir_packet(*packet);
        });
    _logger.debug("IR packet handler registered");
}

void nc::network_handler::_reset_exchange_state() noexcept
{
    // Assumes _state_mutex is held
    _peer_id = nr::badge_unique_id{};
    _ir_timeout_timestamp_ms = 0;
}

void nc::network_handler::_set_ir_protocol_state(
    ir_protocol_state next_state) noexcept
{
    // Assumes _state_mutex is held
    const ir_protocol_state current_state =
        _current_ir_state.load(std::memory_order_relaxed);
    if (current_state == next_state) {
        return;
    }

    _logger.debug("IR state transition: {} -> {}", current_state, next_state);

    _current_ir_state.store(next_state, std::memory_order_relaxed);

    nsec::g::the_badge->lcd_display_ir_exchange_status(next_state);

    if (next_state == ir_protocol_state::IDLE) {
        _reset_exchange_state();
    } else if (next_state == ir_protocol_state::COMPLETED) {
        _handle_successful_sync();
    }
}

void nc::network_handler::_handle_successful_sync() noexcept
{
    if (_peer_id.empty()) {
        _logger.error("Cannot handle successful sync, peer ID is empty");
        return;
    }

    try {
        np::badge_store store;

        if (store.has_id(_peer_id)) {
            _logger.info("Peer already synced {}", _peer_id);
        } else {
            _logger.info("New peer synced {}", _peer_id);
            store.save_id(_peer_id);
            nsec::g::the_badge->apply_score_change(1);
        }
    } catch (const nsec::exception::runtime_error &e) {
        _logger.error("Failed during post-sync processing for peer {}: {}",
                      _peer_id, e.what());
    } catch (...) {
        _logger.error(
            "Unknown exception during post-sync processing for peer {}",
            _peer_id);
    }
}

void nc::network_handler::start_ir_key_exchange() noexcept
{
    std::lock_guard<std::mutex> lock(_state_mutex);

    const ir_protocol_state current_state =
        _current_ir_state.load(std::memory_order_relaxed);
    if (current_state != ir_protocol_state::IDLE) {
        _logger.warn("Cannot start IR exchange from state {}",
                     static_cast<int>(current_state));
        return;
    }

    _logger.info("Starting IR exchange sequence");
    _reset_exchange_state();
    _set_ir_protocol_state(ir_protocol_state::WAITING_FOR_PEER);

    // Stagger initial transmission slightly to reduce collision probability
    uint32_t delay_ms = esp_random() % cfg::ir_sync_request_jitter_max;
    if (delay_ms > 0) {
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }

    _ir_timeout_timestamp_ms =
        get_current_time_ms() + get_timeout_duration_ms();
    _send_ir_packet(message::ir_packet_type::SYNC_REQUEST);
}

void nc::network_handler::_send_ir_packet(message::ir_packet_type type) noexcept
{
    // Assumes _state_mutex is held
    message::ir_packet packet{};
    packet.type = type;

    const nr::badge_unique_id our_id = nsec::g::the_badge->get_unique_id();
    static_assert(sizeof(packet.mac) == sizeof(nr::badge_unique_id),
                  "Packet MAC size must match badge unique ID size");
    memcpy(packet.mac, our_id.data(), sizeof(packet.mac));

    packet.checksum = calculate_checksum(packet);

    _logger.debug("Sending IR packet: type={}, checksum={:#04x}", type,
                  packet.checksum);
    try {
        _ir_interface.send(reinterpret_cast<const uint8_t *>(&packet),
                           sizeof(packet));
        _logger.debug("Sent IR packet successfully");
    } catch (const nsec::exception::runtime_error &e) {
        _logger.error("Send failed for packet type {}: {}",
                      static_cast<int>(type), e.what());
        // Timeout mechanism will handle state recovery if needed
    } catch (...) {
        _logger.error("Unknown exception sending IR packet type {}",
                      static_cast<int>(type));
    }
}

void nc::network_handler::_handle_received_ir_packet(
    const message::ir_packet &packet) noexcept
{
    std::lock_guard<std::mutex> lock(_state_mutex);

    nr::badge_unique_id sender_id;
    static_assert(sizeof(packet.mac) == sizeof(nr::badge_unique_id),
                  "Packet MAC size must match badge unique ID size");
    memcpy(sender_id.data(), packet.mac, sizeof(sender_id));

    const ir_protocol_state current_state =
        _current_ir_state.load(std::memory_order_relaxed);

    _logger.debug("Processing IR packet: type={}, sender={}, current_state={}",
                  packet.type, sender_id, current_state);

    if (current_state == ir_protocol_state::COMPLETED ||
        current_state == ir_protocol_state::TIMEOUT ||
        current_state == ir_protocol_state::CANCELLED) {
        _logger.debug("Ignoring packet in terminal state {}", current_state);
        return;
    }

    bool is_active_exchange = (current_state == ir_protocol_state::SENDER ||
                               current_state == ir_protocol_state::RECEIVER);

    // Check for CANCEL packet during an active exchange
    if (packet.type == message::ir_packet_type::CANCEL) {
        if (is_active_exchange && sender_id == _peer_id) {
            _logger.info("Received {} from peer {}, transitioning to {}",
                         static_cast<int>(packet.type), sender_id,
                         static_cast<int>(ir_protocol_state::CANCELLED));
            _set_ir_protocol_state(ir_protocol_state::CANCELLED);
            _ir_timeout_timestamp_ms =
                get_current_time_ms() + get_idle_delay_ms();
        } else {
            _logger.debug("Ignoring CANCEL packet (active={}, peer_match={})",
                          is_active_exchange, sender_id == _peer_id);
        }
        return;
    }

    if (is_active_exchange && sender_id != _peer_id) {
        _logger.warn(
            "Ignoring packet from unexpected peer {} in state {} (expected {})",
            sender_id, static_cast<int>(current_state), _peer_id);
        return;
    }

    /* State machine logic */
    switch (current_state) {
    case ir_protocol_state::IDLE:
        _logger.warn("Ignoring packet {} in state {}",
                     static_cast<int>(packet.type),
                     static_cast<int>(current_state));
        break;

    case ir_protocol_state::WAITING_FOR_PEER:
        if (packet.type == message::ir_packet_type::SYNC_REQUEST) {
            nr::badge_unique_id our_id = nsec::g::the_badge->get_unique_id();
            bool i_am_lower =
                memcmp(our_id.data(), sender_id.data(), sizeof(our_id)) < 0;

            _logger.info("Received {} from {}", static_cast<int>(packet.type),
                         sender_id);

            if (i_am_lower) {
                _peer_id = sender_id;
                _ir_timeout_timestamp_ms =
                    get_current_time_ms() + get_timeout_duration_ms();
                _set_ir_protocol_state(ir_protocol_state::RECEIVER);
                _send_ir_packet(message::ir_packet_type::SYNC_ACCEPT);
            } else {
                // Higher ID waits for SYNC_ACCEPT
                _ir_timeout_timestamp_ms =
                    get_current_time_ms() + get_timeout_duration_ms();
            }
        } else if (packet.type == message::ir_packet_type::SYNC_ACCEPT) {
            _logger.info("Received {} from {}, becoming {}",
                         static_cast<int>(packet.type), sender_id,
                         static_cast<int>(ir_protocol_state::SENDER));

            _peer_id = sender_id;
            _ir_timeout_timestamp_ms =
                get_current_time_ms() + get_timeout_duration_ms();
            _set_ir_protocol_state(ir_protocol_state::SENDER);
            _send_ir_packet(message::ir_packet_type::ID_DATA);
        } else {
            _logger.debug("Ignoring packet type {} in state {}", packet.type,
                          current_state);
        }
        break;

    case ir_protocol_state::SENDER:
        if (packet.type == message::ir_packet_type::ACK) {
            _logger.info("Received {} from peer {}",
                         static_cast<int>(packet.type), _peer_id);

            _set_ir_protocol_state(ir_protocol_state::COMPLETED);
            _ir_timeout_timestamp_ms =
                get_current_time_ms() + get_idle_delay_ms();
        } else if (packet.type == message::ir_packet_type::SYNC_ACCEPT) {
            // Peer might not have received ID_DATA, resend it
            _logger.info("Received duplicate {} as {}, resending {}",
                         static_cast<int>(packet.type),
                         static_cast<int>(current_state),
                         static_cast<int>(message::ir_packet_type::ID_DATA));

            _ir_timeout_timestamp_ms =
                get_current_time_ms() + get_timeout_duration_ms();
            _send_ir_packet(message::ir_packet_type::ID_DATA);
        } else {
            _logger.debug("Ignoring packet type {} in state {}", packet.type,
                          current_state);
        }
        break;

    case ir_protocol_state::RECEIVER:
        if (packet.type == message::ir_packet_type::ID_DATA) {
            _logger.info("Received {} from peer {}, sending {}",
                         static_cast<int>(packet.type), _peer_id,
                         static_cast<int>(message::ir_packet_type::ACK));

            _ir_timeout_timestamp_ms =
                get_current_time_ms() + get_timeout_duration_ms();
            _send_ir_packet(message::ir_packet_type::ACK);
            _set_ir_protocol_state(ir_protocol_state::COMPLETED);
            _ir_timeout_timestamp_ms =
                get_current_time_ms() + get_idle_delay_ms();
        } else if (packet.type == message::ir_packet_type::SYNC_REQUEST) {
            // Peer might not have received SYNC_ACCEPT, resend it
            _logger.info(
                "Received duplicate {} as {}, resending {}",
                static_cast<int>(packet.type), static_cast<int>(current_state),
                static_cast<int>(message::ir_packet_type::SYNC_ACCEPT));

            _ir_timeout_timestamp_ms =
                get_current_time_ms() + get_timeout_duration_ms();
            _send_ir_packet(message::ir_packet_type::SYNC_ACCEPT);
        } else {
            _logger.debug("Ignoring packet type {} in state {}", packet.type,
                          current_state);
        }
        break;

    case ir_protocol_state::COMPLETED:
    case ir_protocol_state::TIMEOUT:
    case ir_protocol_state::CANCELLED:
        _logger.error("Packet received unexpectedly in final state {}",
                      static_cast<int>(current_state));
        break;
    }
}

void nc::network_handler::_timer_callback(void *arg)
{
    static_cast<nc::network_handler *>(arg)->_check_ir_timeouts();
}

void nc::network_handler::_check_ir_timeouts() noexcept
{
    std::lock_guard<std::mutex> lock(_state_mutex);

    // Check if timeout is active and has expired
    if (_ir_timeout_timestamp_ms <= 0 ||
        get_current_time_ms() < _ir_timeout_timestamp_ms) {
        return;
    }

    const ir_protocol_state current_state =
        _current_ir_state.load(std::memory_order_relaxed);
    _logger.debug("IR timeout check triggered in state {}", current_state);

    switch (current_state) {
    case ir_protocol_state::WAITING_FOR_PEER:
    case ir_protocol_state::SENDER:
    case ir_protocol_state::RECEIVER:
        _logger.warn("IR exchange timed out while in state {}",
                     static_cast<int>(current_state));

        // Send CANCEL if we were actively exchanging with a known peer
        if (current_state == ir_protocol_state::SENDER ||
            current_state == ir_protocol_state::RECEIVER) {
            _send_ir_packet(message::ir_packet_type::CANCEL);
        }

        _set_ir_protocol_state(ir_protocol_state::TIMEOUT);
        // Set a short timeout to transition from TIMEOUT back to IDLE
        _ir_timeout_timestamp_ms = get_current_time_ms() + get_idle_delay_ms();
        break;

    case ir_protocol_state::COMPLETED:
    case ir_protocol_state::TIMEOUT:
    case ir_protocol_state::CANCELLED:
        _logger.info("Idle timeout expired in state {}, returning to {}",
                     static_cast<int>(current_state),
                     static_cast<int>(ir_protocol_state::IDLE));
        _set_ir_protocol_state(ir_protocol_state::IDLE);
        // _ir_timeout_timestamp_ms is reset via _reset_exchange_state on IDLE
        break;

    case ir_protocol_state::IDLE:
        // Timeout should not be active in IDLE state
        _logger.warn("Timeout occurred unexpectedly in state {}",
                     static_cast<int>(current_state));
        _ir_timeout_timestamp_ms = 0;
        break;
    }
}
