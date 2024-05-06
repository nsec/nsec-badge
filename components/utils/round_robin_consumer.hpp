/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_ROUND_ROBIN_CONSUMER_HPP
#define NSEC_ROUND_ROBIN_CONSUMER_HPP

#include <array>
#include <cstdint>

namespace nsec::utils
{

/**
 * A template-based consumer class that manages the round-robin
 * consumption of elements from multiple arrays.
 *
 * This class is designed to allow sequential access to elements from a fixed
 * number of arrays in a round-robin fashion. It ensures that as long as there
 * is an available element in any of the arrays, it can be accessed in the order
 * specified by the round-robin algorithm. Once an array is exhausted, the class
 * automatically skips it and continues with the remaining arrays, thereby
 * managing varying array lengths safely.
 *
 * ArrayCount: The number of arrays to be managed, which determines the
 * size of internal tracking structures.
 *
 * Usage:
 * - Instantiate the class with a std::array of array lengths corresponding to
 *   each managed array.
 * - Call `consume()` to retrieve the next available element according to the
 *   round-robin order. The method returns an `element_description` struct,
 *   which contains the array_id and the index of the element in that array.
 * - Continue calling `consume()` to sequentially access elements until all
 *   arrays are exhausted.
 *
 * Example:
 *   std::array<std::size_t, 3> lengths = {3, 5, 2};
 *   array_round_robin_consumer<3> consumer(lengths);
 *   auto element = consumer.consume(); // Get an element descriptor that
 *                                      // indicates the next element to consume
 *
 * Note:
 * - The class assumes that the input array lengths accurately represent the
 *   number of accessible elements in each array.
 * - The class does not manage the actual arrays of elements but expects that
 *   they are maintained externally and are accessible via their indices as
 *   described by the consumed `element_description`s.
 * - The class expects the user to not call `consume()` more often than the
 *   sum of all elements contained in the tracked arrays.
 */
template <unsigned int ArrayCount> class array_round_robin_consumer
{
  public:
    // Struct representing a particular array and its index
    struct element_description {
        std::size_t array_id; // ID of the array
        std::size_t index;    // Index within the array
    };

    // Constructor initializes with an array specifying the length of each array
    explicit array_round_robin_consumer(
        const std::array<std::size_t, ArrayCount> &array_lengths)
        : _array_lengths{array_lengths}
    // Initialize with the input array lengths
    {
    }

    // Method to get the next available element in round-robin fashion
    element_description consume()
    {
        // Loop through the arrays, attempting to find a valid element
        for (unsigned int attempt = 0; attempt < ArrayCount; attempt++) {
            const auto array_id = _current_array_index;

            // Retrieve the current element description
            const auto element_description = _get_current_element();

            // Check if the current element is valid (not beyond the length)
            if (_is_valid_element(element_description)) {
                // Move to the next element within the current array
                _move_to_next_element(array_id);

                // Move to the next array in a round-robin manner
                _move_to_next_array(array_id);

                // Return the valid element description
                return element_description;
            }

            // Move to the next array if the current one is exhausted
            _move_to_next_array(array_id);
        }

        // If no valid element is found, return a default/invalid value
        return {.array_id = 0, .index = 0};
    }

  private:
    // Move to the next array in round-robin fashion
    void _move_to_next_array(std::size_t array_id)
    {
        _current_array_index = (array_id + 1) % ArrayCount;
    }

    // Move to the next element in the specified array
    void _move_to_next_element(std::size_t array_id)
    {
        _array_positions[array_id]++;
    }

    // Check if the element described is valid based on its index and array
    // length
    bool _is_valid_element(const element_description &element)
    {
        return element.index < _array_lengths[element.array_id];
    }

    // Get the description (array ID and index) of the current element
    element_description _get_current_element()
    {
        return {.array_id = _current_array_index,
                .index = _array_positions[_current_array_index]};
    }

    // Lengths of each input array
    const std::array<std::size_t, ArrayCount> &_array_lengths;

    // Track the current index of each array
    std::array<std::size_t, ArrayCount> _array_positions = {};

    // Track the current array being processed
    std::size_t _current_array_index = 0;
};
} // namespace nsec::utils
#endif /* NSEC_ROUND_ROBIN_CONSUMER_HPP */