/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_FREERTOS_LOCK_HPP
#define NSEC_FREERTOS_LOCK_HPP

#include "exception.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <mutex>

namespace nsec {
namespace synchro {

namespace internal {
/*
 * Class wrapping FreeRTOS mutexes and satisfying the Mutex named requirements, except
 * for the "Default Constructible" requirement.
 */
class mutex {
public:
	explicit mutex(SemaphoreHandle_t& sem) : _mutex(sem)
	{
	}

	~mutex() = default;

	/* "Not copyable" and "not moveable" Mutex requirements. */
	mutex(mutex const&) = delete;
	mutex(mutex&&) = delete;
	mutex& operator=(mutex const&) = delete;
	mutex& operator=(mutex&&) = delete;

	void lock()
	{
		if (xSemaphoreTake(_mutex, portMAX_DELAY) != pdTRUE) {
			NSEC_THROW_ERROR("Failed to try to lock mutex");
		}
	}

	void unlock()
	{
                xSemaphoreGive(_mutex);
	}

private:
	SemaphoreHandle_t& _mutex;
};
} /* namespace internal */

/*
 * Provides the basic concept of std::lock_guard for FreeRTOS binary semaphores.
 *
 * `lock` is held for the duration of lock_guard's lifetime.
 */
class lock_guard {
public:
	explicit lock_guard(SemaphoreHandle_t& mutex) : _mutex(mutex), _guard(_mutex)
	{
	}

	~lock_guard() = default;

	lock_guard(const lock_guard&) = delete;
	lock_guard(lock_guard&&) = delete;
	lock_guard& operator=(const lock_guard&) = delete;
	lock_guard& operator=(lock_guard&&) = delete;

private:
	internal::mutex _mutex;
	std::lock_guard<internal::mutex> _guard;
};

} /* namespace synchro */
} /* namespace nsec */

#endif /* NSEC_FREERTOS_LOCK_HPP */