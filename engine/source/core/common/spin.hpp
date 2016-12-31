#pragma once

#include <atomic>

namespace core
{
	struct spin_mutex
	{
		spin_mutex() = default;
		spin_mutex(const spin_mutex&) = delete;
		spin_mutex& operator = (const spin_mutex&) = delete;

		void lock()
		{
			while (_lock.test_and_set(std::memory_order_acquire)) {}
		}

		void unlock()
		{
			_lock.clear(std::memory_order_release);
		}

	protected:
		std::atomic_flag _lock = ATOMIC_FLAG_INIT;
	};
}