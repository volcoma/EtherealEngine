#pragma once
#include <future>
#include <functional>
#include <deque>
#include <mutex>
#include <string>
#include <memory>

namespace tasks
{
	namespace detail
	{
		struct future_wrapper
		{
			virtual ~future_wrapper() = default;
		};

		template<typename T>
		struct future_wrapper_t : public future_wrapper
		{
			std::shared_future<T> future;
		};

		class future_cache
		{
		public:
			template<typename T>
			std::shared_future<T> cache(std::future<T>& f)
			{
				auto shared = f.share();
				auto wrapper = std::make_unique<future_wrapper_t<T>>();
				wrapper->future = shared;
				
				std::lock_guard<std::mutex> lock(futures_mutex);
				futures.emplace_back(std::move(wrapper));
				return shared;
			}
		private:
			std::mutex futures_mutex;
			std::vector<std::unique_ptr<future_wrapper>> futures;
		};

		future_cache& get_future_cache()
		{
			static future_cache cache;
			return cache;
		}
		

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

	template<class T, class Function>
	std::shared_future<typename std::result_of<Function(std::shared_future<T>&)>::type>
		then(std::launch policy, std::shared_future<T>& fut, Function&& f)
	{
		return detail::get_future_cache().cache(std::async(policy, [](std::shared_future<T>&& fut, Function&& f)
		{
			fut.wait();
			return std::forward<Function>(f)(fut);
		},
		std::move(fut),
		std::forward<Function>(f)
		));
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
		return then(policy, fut.share(), std::forward<Function>(f));
	}

	template<class T, class Function>
	std::shared_future<typename std::result_of<Function(std::shared_future<T>&)>::type>
		then(std::future<T>& fut, Function&& f)
	{
		return then(std::launch::async | std::launch::deferred, fut.share(), std::forward<Function>(f));
	}

	template<class T, class Function>
	std::shared_future<typename std::result_of<Function(std::shared_future<T>&)>::type>
		then_on_main(std::launch policy, std::shared_future<T>& fut, Function&& f)
	{
		return detail::get_future_cache().cache(std::async(policy, [](std::shared_future<T>&& fut, Function&& f)
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
		));
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
		return then_on_main(policy, fut.share(), std::forward<Function>(f));
	}

	template<class T, class Function>
	std::shared_future<typename std::result_of<Function(std::shared_future<T>&)>::type>
		then_on_main(std::future<T>& fut, Function&& f)
	{
		return then_on_main(std::launch::async | std::launch::deferred, fut.share(), std::forward<Function>(f));
	}
	
	template<typename F, typename ...Args>
	auto async(F&& f, Args&& ... args) -> 
		decltype(detail::get_future_cache().cache(std::async(std::forward<F>(f), std::forward<Args>(args)...)))
	{
		return detail::get_future_cache().cache(std::async(std::forward<F>(f), std::forward<Args>(args)...));
	}

	template<typename F, typename ...Args>
	auto async(std::launch policy, F&& f, Args&& ... args) ->
		decltype(detail::get_future_cache().cache(std::async(policy, std::forward<F>(f), std::forward<Args>(args)...)))
	{
		return detail::get_future_cache().cache(std::async(policy, std::forward<F>(f), std::forward<Args>(args)...))
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