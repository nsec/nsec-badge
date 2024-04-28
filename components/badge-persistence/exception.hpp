/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_BADGE_PERSISTENCE_EXCEPTION_HPP
#define NSEC_BADGE_PERSISTENCE_EXCEPTION_HPP

#include <utils/exception.hpp>

#define NSEC_PERSISTENCE_THROW_ERROR_CORRUPTED(msg)                            \
    throw nsec::persistence::exception::corrupted_storage(msg, __FILE__,       \
                                                          __func__, __LINE__)

#define NSEC_PERSISTENCE_THROW_ERROR(msg)                                      \
    throw nsec::persistence::exception::error(msg, __FILE__, __func__, __LINE__)

namespace nsec::persistence::exception
{

// Corrupted storage error
class corrupted_storage : public nsec::exception::runtime_error
{
  public:
    explicit corrupted_storage(const std::string &msg, const char *file_name,
                               const char *function_name,
                               unsigned int line_number)
        : nsec::exception::runtime_error(msg, file_name, function_name,
                                         line_number)
    {
    }
};

// General error
class error : public nsec::exception::runtime_error
{
  public:
    explicit error(const std::string &msg, const char *file_name,
                   const char *function_name, unsigned int line_number)
        : nsec::exception::runtime_error(msg, file_name, function_name, line_number)
    {
    }
};

} // namespace nsec::persistence::exception

#endif // NSEC_BADGE_PERSISTENCE_EXCEPTION_HPP
