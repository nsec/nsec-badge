/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include <badge-persistence/badge_store.hpp>
#include <exception.hpp>
#include <utils/config.hpp>

namespace np = nsec::persistence;

np::badge_store::badge_store() noexcept : _logger{"Badge store"}
{
}

np::utils::managed_nvs_handle np::badge_store::_open_nvs_handle()
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

unsigned int np::badge_store::count()
{

    np::utils::initialize_storage();

    auto nvs = _open_nvs_handle();

    std::uint16_t count_value;

    const auto read_count_ret = nvs_get_u16(
        nvs, nsec::config::persistence::badge_paired_count_field_name,
        &count_value);
    if (read_count_ret != ESP_OK) {
        _logger.warn("Failed to read value: field_name={}, ret={}",
                     nsec::config::persistence::badge_paired_count_field_name,
                     read_count_ret);
        return 0;
    }

    return count_value;
}

np::badge_store::iterator::value_type
np::badge_store::iterator::operator*() const
{
    np::badge_store::iterator::value_type badge_id;

    auto nvs = np::badge_store::_open_nvs_handle();

    size_t expected_length = badge_id.size();

    const auto badge_id_key_name = fmt::format(
        "{}-{}", nsec::config::persistence::badge_id_field_name_prefix, _index);

    const auto read_id_ret = nvs_get_blob(nvs, badge_id_key_name.c_str(),
                                          badge_id.data(), &expected_length);
    if (read_id_ret != ESP_OK) {
        NSEC_PERSISTENCE_THROW_ERROR_CORRUPTED(fmt::format(
            "Failed to read badge id: field_name={}, ret={}",
            nsec::config::persistence::badge_paired_count_field_name,
            read_id_ret));
    }

    return badge_id;
}

void np::badge_store::save_id(const nsec::runtime::badge_unique_id &id)
{
    const auto badge_count = count();
    const auto id_sequence_number = badge_count;

    auto nvs = np::badge_store::_open_nvs_handle();

    const auto badge_id_key_name = fmt::format(
        "{}-{}", nsec::config::persistence::badge_id_field_name_prefix,
        id_sequence_number);

    // Write new badge id
    const auto write_blob_ret =
        nvs_set_blob(nvs, badge_id_key_name.c_str(), id.data(), id.size());
    if (write_blob_ret != ESP_OK) {
        NSEC_PERSISTENCE_THROW_ERROR(fmt::format(
            "Failed to write badge id to flash: key_name={}, nvs_error={}",
            badge_id_key_name, write_blob_ret));
    }

    // Update total count
    std::uint16_t new_badge_count = std::uint16_t(id_sequence_number + 1);
    const auto write_count_ret = nvs_set_u16(
        nvs, nsec::config::persistence::badge_paired_count_field_name,
        new_badge_count);
    if (write_count_ret != ESP_OK) {
        NSEC_PERSISTENCE_THROW_ERROR(
            fmt::format("Failed to write badge id count to flash: key_name={}, "
                        "count={}, nvs_error={}",
                        badge_id_key_name, new_badge_count, write_count_ret));
    }
}