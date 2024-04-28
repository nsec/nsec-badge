/*
 * Copyright (C) 2022 Jérémie Galarneau <jeremie.galarneau@efficios.com>
 */

#ifndef NSEC_UNIQUE_WRAPPER_H
#define NSEC_UNIQUE_WRAPPER_H

#include <memory>

namespace nsec
{

/*
 * make_unique_wrapper is intended to facilitate the use of std::unique_ptr
 * to wrap C-style APIs that don't provide RAII resource management facilities.
 *
 * Usage example:
 *
 *    // API
 *    struct my_c_struct {
 *            // ...
 *    };
 *
 *    struct my_c_struct *create_my_c_struct(void);
 *    void destroy_my_c_struct(struct my_c_struct *value);
 *
 *    // Creating a unique_ptr to my_c_struct.
 *    auto safe_c_struct =
 *            lttng::make_unique_wrapper<my_c_struct, destroy_my_c_struct>(
 *                    create_my_c_struct());
 *
 * Note that this facility is intended for use in the scope of a function.
 * If you need to return this unique_ptr instance, you should consider writing
 * a proper, idiomatic, wrapper.
 */

namespace memory
{
template <typename WrappedType, void (*DeleterFunction)(WrappedType *)>
struct create_deleter_class {
    struct deleter {
        void operator()(WrappedType *instance) const
        {
            DeleterFunction(instance);
        }
    };

    std::unique_ptr<WrappedType, deleter>
    operator()(WrappedType *instance) const
    {
        return std::unique_ptr<WrappedType, deleter>(instance);
    }
};
} /* namespace memory */

template <typename WrappedType, void (*DeleterFunc)(WrappedType *)>
std::unique_ptr<WrappedType, typename memory::create_deleter_class<
                                 WrappedType, DeleterFunc>::deleter>
make_unique_wrapper(WrappedType *instance = nullptr)
{
    const memory::create_deleter_class<WrappedType, DeleterFunc> unique_deleter;

    return unique_deleter(instance);
}

} // namespace nsec

#endif /* NSEC_UNIQUE_WRAPPER_H */