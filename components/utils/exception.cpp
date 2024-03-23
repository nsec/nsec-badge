/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include "exception.hpp"

namespace
{
std::string format_throw_location(const char *file_name,
                                  const char *function_name,
                                  unsigned int line_number)
{
    std::stringstream location;

    location << "[" << function_name << "()"
             << " " << file_name << ":" << line_number << "]";

    return location.str();
}
} // namespace

nsec::exception::runtime_error::runtime_error(const std::string &msg,
                                              const char *file_name,
                                              const char *function_name,
                                              unsigned int line_number)
    : std::runtime_error(
          msg + " " +
          format_throw_location(file_name, function_name, line_number))
{
}