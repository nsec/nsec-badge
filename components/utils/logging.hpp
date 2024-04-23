/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_LOGGING_HPP
#define NSEC_LOGGING_HPP

#include <fmt/format.h>
#include <freertos/FreeRTOS.h>
#include <iostream>
#include <string_view>

#include "exception.hpp"

namespace nsec::logging
{

class logger
{
  public:
    enum class severity {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
    };

    explicit logger(std::string_view subsystem_name,
                    severity minimal_level = severity::ERROR)
        : _subsystem_name(subsystem_name), _minimal_level(minimal_level)
    {
    }

    template <typename... Args>
    void log(severity severity_level, fmt::format_string<Args...> format,
             Args &&...args) const
    {
        if (severity_level < _minimal_level) {
            return;
        }

        fmt::print("[{}:{} {}] {}\n", _subsystem_name,
                   _severity_name(severity_level), xPortGetCoreID(),
                   fmt::format(format, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void debug(fmt::format_string<Args...> format, Args &&...args) const
    {
        log(severity::DEBUG, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(fmt::format_string<Args...> format, Args &&...args) const
    {
        log(severity::INFO, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(fmt::format_string<Args...> format, Args &&...args) const
    {
        log(severity::WARNING, format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(fmt::format_string<Args...> format, Args &&...args) const
    {
        log(severity::ERROR, format, std::forward<Args>(args)...);
    }

  private:
    const std::string_view _subsystem_name;
    const severity _minimal_level;

    static std::string_view _severity_name(severity severity_level)
    {
        switch (severity_level) {
        case severity::DEBUG:
            return "debug";
        case severity::INFO:
            return "info";
        case severity::WARNING:
            return "warning";
        case severity::ERROR:
            return "error";
        default:
            NSEC_THROW_ERROR(
                fmt::format("Invalid logging severity level: level={}",
                            int(severity_level)));
        }
    }
};
} // namespace nsec::logging
#endif // NSEC_LOGGING_HPP
