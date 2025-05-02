/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 * SPDX-FileCopyrightText: 2025 NorthSec
 */

#ifndef NSEC_BADGE_PERSISTENCE_CONFIG_STORE_HPP
#define NSEC_BADGE_PERSISTENCE_CONFIG_STORE_HPP

#include <utils/board.hpp>
#include <utils/logging.hpp>

#include <optional>

#include <badge-persistence/utils.hpp>

namespace nsec::persistence
{

class config_store
{
  public:
    struct config {
        std::uint8_t selected_animation_id;
        std::uint8_t social_level;
        std::uint16_t sponsor_flag;
        std::uint32_t social_level_check;
    };

    explicit config_store() noexcept;

    // Deactivate copy and assignment.
    config_store(const config_store &) = delete;
    config_store(config_store &&) = delete;
    config_store &operator=(const config_store &) = delete;
    config_store &operator=(config_store &&) = delete;
    ~config_store() = default;

    std::optional<config> load();

    void save_selected_animation_id(std::uint8_t id);
    void save_social_level(std::uint8_t level);
    void save_sponsor_flag(std::uint16_t flag);
    void save_social_level_check(std::uint32_t level_check);

  private:
  utils::managed_nvs_handle _open_nvs_handle();
  void _save_u8_field(const char *field_name, std::uint8_t value);
  void _save_u16_field(const char *field_name, std::uint16_t value);
  void _save_u32_field(const char *field_name, std::uint32_t value);

    nsec::logging::logger _logger;
};

} // namespace nsec::persistence

#endif // NSEC_BADGE_PERSISTENCE_CONFIG_STORE_HPP
