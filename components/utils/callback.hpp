/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_CALLBACK_HPP
#define NSEC_CALLBACK_HPP

namespace nsec
{
// Basic callback class that captures a user argument and forwards invocation
// arguments. Since avr-g++ doesn't provide much in terms of metaprogramming
// support (e.g. std::forward, type_traits, etc.), this implementation is quite
// naive.
template <class FunctionReturnType, class... FunctionArgs> class callback
{
  public:
    using raw_function_type = FunctionReturnType (*)(void *, FunctionArgs...);

    callback(raw_function_type function, void *data) noexcept
        : _function{function}, _user_data{data}
    {
    }

    FunctionReturnType operator()(FunctionArgs... args) const
    {
        return _function(_user_data, args...);
    }

  private:
    raw_function_type _function;
    void *_user_data;
};
} // namespace nsec

#endif // NSEC_CALLBACK_HPP
