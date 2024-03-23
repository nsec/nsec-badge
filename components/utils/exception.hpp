/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_EXCEPTION_HPP
#define NSEC_EXCEPTION_HPP

#include <sstream>
#include <stdexcept>

#define NSEC_THROW_ERROR(msg)                                                  \
    throw nsec::exception::runtime_error(msg, __FILE__, __func__, __LINE__)

namespace nsec::exception
{

// Generic runtime error.
class runtime_error : public std::runtime_error
{
  public:
    explicit runtime_error(const std::string &msg, const char *file_name,
                           const char *function_name, unsigned int line_number);
};

} // namespace nsec::exception

#endif // NSEC_EXCEPTION_HPP
