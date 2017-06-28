#pragma once
#include <future>
#include <functional>
#include <deque>
#include <mutex>
#include <string>

namespace tasks
{
	namespace detail
	{
		std::deque<std::function<void()>>& get_queue()
		{
			static std::deque<std::function<void()>> queue;
			return queue;
		}

		std::mutex& get_queue_mutex()
		{
			static std::mutex queue_mutex;
			return queue_mutex;
		}
	}

	template<typename T>
	std::shared_future<T> make_shared_future(std::future<T>&& f)
	{
		return std::shared_future<T>(std::move(f));
	}
	template<typename T>
	std::shared_future<T> make_shared_future(std::future<T>& f)
	{
		return std::shared_future<T>(std::move(f));
	}

	template<class T, class Function>
	std::shared_future<typename std::result_of<Function(std::shared_future<T>&)>::type>
		then(std::launch policy, std::shared_future<T>& fut, Function&& f)
	{
		return std::async(policy, [](std::shared_future<T>&& fut, Function&& f)
		{
			fut.wait();
			return std::forward<Function>(f)(fut);
		},
		std::move(fut),
		std::forward<Function>(f)
		);
	}

	template<class T, class Function>
	std::shared_future<typename std::result_of<Function(std::shared_future<T>&)>::type>
		then(std::shared_future<T>& fut, Function&& f)
	{
		return then(std::launch::async | std::launch::deferred, fut, std::forward<Function>(f));
	}

	template<class T, class Function>
	std::shared_future<typename std::result_of<Function(std::shared_future<T>&)>::type>
		then(std::launch policy, std::future<T>& fut, Function&& f)
	{
		return then(policy, make_shared_future(fut), std::forward<Function>(f));
	}

	template<class T, class Function>
	std::shared_future<typename std::result_of<Function(std::shared_future<T>&)>::type>
		then(std::future<T>& fut, Function&& f)
	{
		return then(std::launch::async | std::launch::deferred, make_shared_future(fut), std::forward<Function>(f));
	}

	template<class T, class Function>
	std::shared_future<typename std::result_of<Function(std::shared_future<T>&)>::type>
		then_on_main(std::launch policy, std::shared_future<T>& fut, Function&& f)
	{
		return std::async(policy, [](std::shared_future<T>&& fut, Function&& f)
		{
			fut.wait();

			std::lock_guard<std::mutex> lock(detail::get_queue_mutex());
			detail::get_queue().emplace_back([f, fut]() mutable
			{
				f(fut);
			});
		},
		std::move(fut),
		std::forward<Function>(f)
		);
	}

	template<class T, class Function>
	std::shared_future<typename std::result_of<Function(std::shared_future<T>&)>::type>
		then_on_main(std::shared_future<T>& fut, Function&& f)
	{
		return then_on_main(std::launch::async | std::launch::deferred, fut, std::forward<Function>(f));
	}

	template<class T, class Function>
	std::shared_future<typename std::result_of<Function(std::shared_future<T>&)>::type>
		then_on_main(std::launch policy, std::future<T>& fut, Function&& f)
	{
		return then_on_main(policy, make_shared_future(fut), std::forward<Function>(f));
	}

	template<class T, class Function>
	std::shared_future<typename std::result_of<Function(std::shared_future<T>&)>::type>
		then_on_main(std::future<T>& fut, Function&& f)
	{
		return then_on_main(std::launch::async | std::launch::deferred, make_shared_future(fut), std::forward<Function>(f));
	}
	
	template<typename F, typename ...Args>
	auto async(F&& f, Args&& ... args) -> decltype(make_shared_future(std::async(std::forward<F>(f), std::forward<Args>(args)...)))
	{
		return make_shared_future(std::async(std::forward<F>(f), std::forward<Args>(args)...));
	}

	template<typename F, typename ...Args>
	auto async(std::launch policy, F&& f, Args&& ... args) -> decltype(make_shared_future(std::async(policy, std::forward<F>(f), std::forward<Args>(args)...)))
	{
		return make_shared_future(std::async(policy, std::forward<F>(f), std::forward<Args>(args)...));
	}

	template<typename R>
	bool is_ready(std::future<R> const& f)
	{
		return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}

	template<typename R>
	bool is_ready(std::shared_future<R> const& f)
	{
		return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}

	inline bool has_tasks()
	{
		return !detail::get_queue().empty();
	}

	inline void process_task()
	{
		std::function<void()> task;
		{
			std::lock_guard<std::mutex> lock(detail::get_queue_mutex());
			task = detail::get_queue().front();
			detail::get_queue().pop_front();
		}

		if (task)
		{
			task();
		}
	}

	inline void process_tasks()
	{
		while (has_tasks())
		{
			process_task();
		}
	}
}