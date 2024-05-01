/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_BADGE_PERSISTENCE_UTILS_HPP
#define NSEC_BADGE_PERSISTENCE_UTILS_HPP

#include <nvs_flash.h>

#include <utils/unique_wrapper.hpp>

#include <memory>
#include <string_view>

namespace nsec::persistence::utils
{
class managed_nvs_handle
{
  public:
    explicit managed_nvs_handle(nvs_handle_t new_handle) : _handle(new_handle)
    {
    }

    ~managed_nvs_handle()
    {
        if (_handle) {
            nvs_commit(_handle);
        }

        nvs_close(_handle);
    }

    // Prevent copy operations
    managed_nvs_handle(const managed_nvs_handle &) = delete;
    managed_nvs_handle &operator=(const managed_nvs_handle &) = delete;

    // Enable move operations
    managed_nvs_handle(managed_nvs_handle &&other) noexcept
        : _handle(other._handle)
    {
        other._handle = 0;
    }

    managed_nvs_handle &operator=(managed_nvs_handle &&other) noexcept
    {
        if (this != &other) {
            nvs_close(_handle);
            _handle = other._handle;
            other._handle = 0;
        }

        return *this;
    }

    nvs_handle_t get() const
    {
        return _handle;
    }

    // Implicit cast operator
    operator nvs_handle_t() const
    {
        return _handle;
    }

  private:
    nvs_handle_t _handle;
};

void initialize_storage();
void reset_storage();

} // namespace nsec::persistence::utils

#endif // NSEC_BADGE_PERSISTENCE_UTILS_HPP
