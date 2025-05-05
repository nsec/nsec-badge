/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_BADGE_PERSISTENCE_BADGE_STORE_HPP
#define NSEC_BADGE_PERSISTENCE_BADGE_STORE_HPP

#include <badge/id.hpp>
#include <badge-persistence/utils.hpp>
#include <utils/board.hpp>
#include <utils/logging.hpp>

#include <array>
#include <optional>

namespace nsec::persistence
{

class badge_store
{
  public:
    class iterator;
    friend iterator;
    explicit badge_store() noexcept;

    // Deactivate copy and assignment.
    badge_store(const badge_store &) = delete;
    badge_store(badge_store &&) = delete;
    badge_store &operator=(const badge_store &) = delete;
    badge_store &operator=(badge_store &&) = delete;
    ~badge_store() = default;

    class iterator
    {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = nsec::runtime::badge_unique_id;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

        explicit iterator(int index) : _index(index)
        {
        }

        value_type operator*() const;

        iterator &operator++()
        {
            ++_index;
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const iterator &a, const iterator &b)
        {
            return a._index == b._index;
        }

        friend bool operator!=(const iterator &a, const iterator &b)
        {
            return a._index != b._index;
        }

      private:
        int _index;
    };

    unsigned int count();
    void save_id(const nsec::runtime::badge_unique_id &id);
    bool has_id(const nsec::runtime::badge_unique_id &id);

    iterator begin()
    {
        return iterator(0);
    }
    iterator end()
    {
        return iterator(count());
    }

  private:
    static utils::managed_nvs_handle _open_nvs_handle();

    nsec::logging::logger _logger;
};

} // namespace nsec::persistence

#endif // NSEC_BADGE_PERSISTENCE_BADGE_STORE_HPP
