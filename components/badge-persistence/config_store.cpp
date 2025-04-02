/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include <badge-persistence/config_store.hpp>
#include <badge-persistence/exception.hpp>
#include <badge-persistence/utils.hpp>

#include <utils/config.hpp>

#include <nvs_flash.h>

namespace np = nsec::persistence;

np::config_store::config_store() noexcept : _logger{"Config store"}
{
}

std::optional<np::config_store::config> np::config_store::load()
{
    np::utils::initialize_storage();

    auto nvs = _open_nvs_handle();

    config cfg;
    const auto read_social_ret =
        nvs_get_u8(nvs, nsec::config::persistence::social_level_field_name,
                   &cfg.social_level);
    if (read_social_ret != ESP_OK) {
        _logger.warn("Failed to read value: field_name={}, ret={}",
                     nsec::config::persistence::social_level_field_name,
                     read_social_ret);
        return std::nullopt;
    }

    auto read_animation_ret = nvs_get_u8(
        nvs, nsec::config::persistence::selected_animation_field_name,
        &cfg.selected_animation_id);
    if (read_social_ret != ESP_OK) {
        _logger.warn("Failed to read value: field_name={}, ret={}",
                     nsec::config::persistence::selected_animation_field_name,
                     read_animation_ret);
        return std::nullopt;
    }

    auto read_sponsor_ret = nvs_get_u16(
        nvs, nsec::config::persistence::sponsor_flag_field_name,
        &cfg.sponsor_flag);
    if (read_social_ret != ESP_OK) {
        _logger.warn("Failed to read value: field_name={}, ret={}",
                     nsec::config::persistence::sponsor_flag_field_name,
                     read_sponsor_ret);
        return std::nullopt;
    }

    return cfg;
}

np::utils::managed_nvs_handle np::config_store::_open_nvs_handle()
{

    nvs_handle_t raw_nvs_handle;
    const auto ret = nvs_open(nsec::config::persistence::namespace_name,
                              NVS_READWRITE, &raw_nvs_handle);

    if (ret != ESP_OK) {
        NSEC_PERSISTENCE_THROW_ERROR(
            fmt::format("Failed to open namespace: namespace={}",
                        nsec::config::persistence::namespace_name));
    }

    return np::utils::managed_nvs_handle(raw_nvs_handle);
}

void np::config_store::save_selected_animation_id(std::uint8_t id)
{
    _save_u8_field(nsec::config::persistence::selected_animation_field_name,
                   id);
}

void np::config_store::save_social_level(std::uint8_t level)
{
    _save_u8_field(nsec::config::persistence::social_level_field_name, level);
}

void np::config_store::save_sponsor_flag(std::uint16_t level)
{
    _save_u16_field(nsec::config::persistence::sponsor_flag_field_name, level);
}

void np::config_store::_save_u8_field(const char *field_name,
                                      std::uint8_t value)
{
    auto nvs = _open_nvs_handle();

    _logger.info("Saving field to namespace: namespace_name={}, "
                 "field_name={}, value={}",
                 nsec::config::persistence::namespace_name, field_name, value);

    const auto write_ret = nvs_set_u8(nvs, field_name, value);
    if (write_ret != ESP_OK) {
        _logger.error("Failed to save field to namespace: namespace_name={}, "
                      "field_name={}, value={}, nvs_error={}",
                      nsec::config::persistence::namespace_name, field_name,
                      value, write_ret);
    }
}

void np::config_store::_save_u16_field(const char *field_name,
                                      std::uint16_t value)
{
    auto nvs = _open_nvs_handle();

    _logger.info("Saving field to namespace: namespace_name={}, "
                 "field_name={}, value={}",
                 nsec::config::persistence::namespace_name, field_name, value);

    const auto write_ret = nvs_set_u16(nvs, field_name, value);
    if (write_ret != ESP_OK) {
        _logger.error("Failed to save field to namespace: namespace_name={}, "
                      "field_name={}, value={}, nvs_error={}",
                      nsec::config::persistence::namespace_name, field_name,
                      value, write_ret);
    }
}
