#ifndef TASK_SYSTEM_H
#define TASK_SYSTEM_H

#include "../common/nonstd/function_traits.hpp"
#include "../common/nonstd/type_traits.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <exception>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
namespace core
{
class task_system;

template <typename T>
class task_future
{
public:
	decltype(auto) get() const
	{
		wait();

		return future.get();
	}

	decltype(auto) valid() const
	{
		return future.valid();
	}
	bool is_ready() const
	{
		using namespace std::chrono_literals;
		return valid() && wait_for(0s) == std::future_status::ready;
	}

	//-----------------------------------------------------------------------------
	//  Name : wait ()
	/// <summary>
	/// Waits for a task in a smart way. Allows processing of other task on this
	/// thread. The task is guaranteed to be ready when this function exits if
	/// the task is executed by the owner thread or any of the worker threads that
	/// we manage.
	/// </summary>
	//-----------------------------------------------------------------------------
	void wait() const;

	template <class Rep, class Per>
	std::future_status wait_for(const std::chrono::duration<Rep, Per>& rel_time) const
	{ // wait for duration
		return future.wait_for(rel_time);
	}

	template <class Clock, class Dur>
	std::future_status wait_until(const std::chrono::time_point<Clock, Dur>& abs_time) const
	{ // wait until time point
		return future.wait_until(abs_time);
	}

	static task_future<T> from_shared_future(const std::shared_future<T>& fut)
	{
		task_future<T> res;
		res.future = fut;
		return res;
	}
	static task_future<T> from_shared_future(std::shared_future<T>&& fut)
	{
		task_future<T> res;
		res.future = std::move(fut);
		return res;
	}
	static task_future<T> from_future(std::future<T>&& fut)
	{
		task_future<T> res;
		res.future = fut.share();
		return res;
	}

private:
	friend class task_system;
	std::shared_future<T> future;
	task_system* _system = nullptr;
};

template <class>
struct is_future : std::false_type
{
};

template <class T>
struct is_future<std::future<T>> : std::true_type
{
};

template <class T>
struct is_future<std::shared_future<T>> : std::true_type
{
};
template <class T>
struct is_future<std::shared_future<T>&> : std::true_type
{
};
template <class T>
struct is_future<const std::shared_future<T>&> : std::true_type
{
};

template <class T>
struct is_future<task_future<T>> : std::true_type
{
};
template <class T>
struct is_future<task_future<T>&> : std::true_type
{
};
template <class T>
struct is_future<const task_future<T>&> : std::true_type
{
};

template <class T>
struct decay_future
{
	using type = T;
};

template <class T>
struct decay_future<std::future<T>>
{
	using type = T;
};

template <class T>
struct decay_future<std::shared_future<T>>
{
	using type = T;
};

template <class T>
struct decay_future<std::shared_future<T>&>
{
	using type = T;
};

template <class T>
struct decay_future<const std::shared_future<T>&>
{
	using type = T;
};

template <class T>
struct decay_future<task_future<T>>
{
	using type = T;
};

template <class T>
struct decay_future<task_future<T>&>
{
	using type = T;
};

template <class T>
struct decay_future<const task_future<T>&>
{
	using type = T;
};

/*
 * awaitable_task; a type-erased, allocator-aware std::packaged_task that
 * also contains its own arguments. The underlying packaged_task and the
 * stored argument tuple can be heap allocated or allocated with a provided
 * allocator.
 *
 * There are two forms of tasks: ready tasks and awaitable tasks.
 *
 *      Ready tasks are assumed to be immediately invokable; that is,
 *      invoking the underlying pakcaged_task with the provided arguments
 *      will not block. This is contrasted with awaitable tasks where some or
 *      all of the provided arguments may be futures waiting on results of
 *      other tasks.
 *
 *      Awaitable tasks are assumed to take arguments where some or all are
 *      backed by futures waiting on results of other tasks. This is
 *      contrasted with ready tasks that are assumed to be immediately
 *      invokable.
 *
 * There are two helper methods for creating task objects:
 * make_ready_task and make_awaitable_task, both of which return a pair of
 * the newly constructed task and a std::future object to the
 * return value.
 */
class task
{
	template <class T>
	using decay_if_future = typename std::conditional<is_future<typename std::decay<T>::type>::value,
													  typename decay_future<T>::type, T>::type;

	struct ready_task_tag
	{
	};
	struct awaitable_task_tag
	{
	};

public:
	task() = default;
	~task() = default;

	task(task const&) = delete;
	task(task&&) noexcept = default;

	task& operator=(task const&) = delete;
	task& operator=(task&&) noexcept = default;

	void swap(task& other) noexcept
	{
		std::swap(_t, other._t);
	}

	operator bool() const noexcept
	{
		return static_cast<bool>(_t);
	}

	friend class task_system;

	template <class F, class... Args>
	friend std::pair<task, task_future<typename std::result_of<F(Args...)>::type>>
	make_ready_task(F&& f, Args&&... args)
	{
		using pair_type = std::pair<task, task_future<typename std::result_of<F(Args...)>::type>>;
		using model_type = ready_task_model<typename std::result_of<F(Args...)>::type(Args...)>;

		task t(ready_task_tag(), std::forward<F>(f), std::forward<Args>(args)...);
		auto fut = static_cast<model_type&>(*t._t).get_future();
		return pair_type(std::move(t), std::move(fut));
	}

	template <class Allocator, class F, class... Args>
	friend std::pair<task, task_future<typename std::result_of<F(Args...)>::type>>
	make_ready_task(std::allocator_arg_t, Allocator const& alloc, F&& f, Args&&... args)
	{
		using pair_type = std::pair<task, task_future<typename std::result_of<F(Args...)>::type>>;
		using model_type = ready_task_model<typename std::result_of<F(Args...)>::type(Args...)>;

		task t(ready_task_tag(), std::allocator_arg_t(), alloc, std::forward<F>(f),
			   std::forward<Args>(args)...);
		auto fut = static_cast<model_type&>(*t._t).get_future();
		return pair_type(std::move(t), std::move(fut));
	}

	template <class F, class... Args>
	friend std::pair<task, task_future<typename std::result_of<F(decay_if_future<Args>...)>::type>>
	make_awaitable_task(F&& f, Args&&... args)
	{
		using pair_type =
			std::pair<task, task_future<typename std::result_of<F(decay_if_future<Args>...)>::type>>;
		using model_type = awaitable_task_model<
			typename std::result_of<F(decay_if_future<Args>...)>::type(decay_if_future<Args>...), Args...>;

		task t(awaitable_task_tag(), std::forward<F>(f), std::forward<Args>(args)...);
		auto fut = static_cast<model_type&>(*t._t).get_future();
		return pair_type(std::move(t), std::move(fut));
	}

	template <class Allocator, class F, class... Args>
	friend std::pair<task, task_future<typename std::result_of<F(decay_if_future<Args>...)>::type>>
	make_awaitable_task(std::allocator_arg_t, Allocator const& alloc, F&& f, Args&&... args)
	{
		using pair_type =
			std::pair<task, task_future<typename std::result_of<F(decay_if_future<Args>...)>::type>>;
		using model_type = awaitable_task_model<
			typename std::result_of<F(decay_if_future<Args>...)>::type(decay_if_future<Args>...), Args...>;

		task t(awaitable_task_tag(), std::allocator_arg_t(), alloc, std::forward<F>(f),
			   std::forward<Args>(args)...);
		auto fut = static_cast<model_type&>(*t._t).get_future();
		return pair_type(std::move(t), std::move(fut));
	}

	void operator()()
	{
		if(_t)
			_t->invoke_();
	}

	bool ready() const
	{
		if(_t)
			return _t->ready_();
		else
			return false;
	}

private:
	template <class F, class... Args>
	task(ready_task_tag, F&& f, Args&&... args)
		: _t(new ready_task_model<typename std::result_of<F(Args...)>::type(Args...)>(
			  std::forward<F>(f), std::forward<Args>(args)...))
	{
	}

	template <class Allocator, class F, class... Args>
	task(ready_task_tag, std::allocator_arg_t, Allocator const& alloc, F&& f, Args&&... args)
		: _t(new ready_task_model<typename std::result_of<F(Args...)>::type(Args...)>(
			  std::allocator_arg_t(), alloc, std::forward<F>(f), std::forward<Args>(args)...))
	{
	}

	template <class F, class... Args>
	task(awaitable_task_tag, F&& f, Args&&... args)
		: _t(new awaitable_task_model<
			  typename std::result_of<F(decay_if_future<Args>...)>::type(decay_if_future<Args>...), Args...>(
			  std::forward<F>(f), std::forward<Args>(args)...))
	{
	}

	template <class Allocator, class F, class... Args>
	task(awaitable_task_tag, std::allocator_arg_t, Allocator const& alloc, F&& f, Args&&... args)
		: _t(new awaitable_task_model<
			  typename std::result_of<F(decay_if_future<Args>...)>::type(decay_if_future<Args>...), Args...>(
			  std::allocator_arg_t(), alloc, std::forward<F>(f), std::forward<Args>(args)...))
	{
	}

	struct task_concept
	{
		virtual ~task_concept() noexcept;
		virtual void invoke_() = 0;
		virtual bool ready_() const noexcept = 0;
	};

	template <class>
	struct ready_task_model;

	//-----------------------------------------------------------------------------
	//  Name : ready_task_model ()
	/// <summary>
	/// Ready tasks are assumed to be immediately invokable, that is,
	/// invoking the underlying pakcaged_task with the provided arguments
	/// will not block. This is contrasted with async tasks where some or all
	/// of the provided arguments may be futures waiting on results of other
	/// tasks.
	/// </summary>
	//-----------------------------------------------------------------------------
	template <class R, class... Args>
	struct ready_task_model<R(Args...)> : task_concept
	{
		template <class F>
		explicit ready_task_model(F&& f, Args&&... args)
			: _f(std::forward<F>(f))
			, _args(std::forward<Args>(args)...)
		{
		}

		template <class Allocator, class F>
		explicit ready_task_model(std::allocator_arg_t, Allocator const& alloc, F&& f, Args&&... args)
			: _f(std::allocator_arg_t(), alloc, std::forward<F>(f))
			, _args(std::forward<Args>(args)...)
		{
		}

		task_future<R> get_future()
		{
			return task_future<R>::from_shared_future(_f.get_future().share());
		}

		void invoke_() override
		{
			nonstd::apply(_f, _args);
		}

		bool ready_() const noexcept override
		{
			return true;
		}

	private:
		std::packaged_task<R(Args...)> _f;
		std::tuple<nonstd::special_decay_t<Args>...> _args;
	};

	template <class...>
	struct awaitable_task_model;

	//-----------------------------------------------------------------------------
	//  Name : awaitable_task_model ()
	/// <summary>
	/// Async tasks are assumed to take arguments where some or all are
	/// backed by futures waiting on results of other tasks. This is
	/// contrasted with ready tasks that are assumed to be immediately
	/// invokable.
	/// </summary>
	//-----------------------------------------------------------------------------
	template <class R, class... CallArgs, class... FutArgs>
	struct awaitable_task_model<R(CallArgs...), FutArgs...> : task_concept
	{
		template <class F, class... Args>
		explicit awaitable_task_model(F&& f, Args&&... args)
			: _f(std::forward<F>(f))
			, _args(std::forward<Args>(args)...)
		{
		}

		template <class Allocator, class F, class... Args>
		explicit awaitable_task_model(std::allocator_arg_t, Allocator const& alloc, F&& f, Args&&... args)
			: _f(std::allocator_arg_t(), alloc, std::forward<F>(f))
			, _args(std::forward<Args>(args)...)
		{
		}

		task_future<R> get_future()
		{
			return task_future<R>::from_shared_future(_f.get_future().share());
		}

		void invoke_() override
		{
			constexpr const std::size_t arity = sizeof...(FutArgs);
			do_invoke_(std::make_index_sequence<arity>());
		}

		bool ready_() const noexcept override
		{
			constexpr const std::size_t arity = sizeof...(FutArgs);
			return do_ready_(std::make_index_sequence<arity>());
		}

	private:
		template <class T>
		static inline decltype(auto) call_get(T&& t) noexcept
		{
			return std::forward<T>(t);
		}

		template <class T>
		static inline decltype(auto) call_get(task_future<T>&& t) noexcept
		{
			return t.get();
		}

		template <class T>
		static inline decltype(auto) call_get(std::future<T>&& t) noexcept
		{
			return t.get();
		}

		template <class T>
		static inline decltype(auto) call_get(std::shared_future<T>&& t) noexcept
		{
			return t.get();
		}

		template <std::size_t... I>
		inline void do_invoke_(std::index_sequence<I...>)
		{
			nonstd::invoke(_f, call_get(std::get<I>(std::move(_args)))...);
		}

		template <class T>
		static inline bool call_ready(const T&) noexcept
		{
			return true;
		}

		template <class T>
		static inline bool call_ready(const task_future<T>& t) noexcept
		{
			return t.is_ready();
		}

		template <class T>
		static inline bool call_ready(const std::future<T>& t) noexcept
		{
			using namespace std::chrono_literals;
			return t.valid() && t.wait_for(0s) == std::future_status::ready;
		}

		template <class T>
		static inline bool call_ready(const std::shared_future<T>& t) noexcept
		{
			using namespace std::chrono_literals;
			return t.valid() && t.wait_for(0s) == std::future_status::ready;
		}

		template <std::size_t... I>
		inline bool do_ready_(std::index_sequence<I...>) const noexcept
		{
			return nonstd::check_all_true(call_ready(std::get<I>(_args))...);
		}

		std::packaged_task<R(CallArgs...)> _f;
		std::tuple<nonstd::special_decay_t<FutArgs>...> _args;
	};

	std::unique_ptr<task_concept> _t;
};

class task_system
{
	using duration_t = std::chrono::steady_clock::duration;
	template <typename T>
	friend class task_future;

public:
	struct queue_info
	{
		std::size_t pending_tasks = 0;
	};
	struct system_info
	{
		std::size_t pending_tasks = 0;
		std::vector<queue_info> queue_infos;
	};

	using allocator_t = std::allocator<task>;

	task_system();

	task_system(std::size_t nthreads, allocator_t const& alloc = {});

	//-----------------------------------------------------------------------------
	//  Name : ~task_system ()
	/// <summary>
	/// Notifies threads to finish and joins them.
	/// </summary>
	//-----------------------------------------------------------------------------
	~task_system();

	//-----------------------------------------------------------------------------
	//  Name : run_on_owner_thread ()
	/// <summary>
	/// Process owner thread tasks
	/// </summary>
	//-----------------------------------------------------------------------------
	void run_on_owner_thread(duration_t max_duration = duration_t(0));

	system_info get_info() const;
	//-----------------------------------------------------------------------------
	//  Name : get_owner_thread_idx ()
	/// <summary>
	/// Gets the owner thread index in the list. It is a seperate function since the
	/// owner thread is a special case. Can be used to push a task directly
	/// there.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::size_t get_owner_thread_idx() const
	{
		return 0;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_any_worker_thread_idx ()
	/// <summary>
	/// Gets one of the worker threads id. Can be used to push a task directly
	/// there.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::size_t get_any_worker_thread_idx() const
	{
		return get_most_free_queue_idx(true);
	}

	//-----------------------------------------------------------------------------
	//  Name : get_most_busy_queue_idx ()
	/// <summary>
	/// Gets the most busy queue idx. This is basicly used by the work stealing
	/// mechanism.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::size_t get_most_busy_queue_idx(bool skip_owner) const
	{
		if(_threads_count == 1)
			return get_owner_thread_idx();

		auto info = get_info();
		auto begin_it = skip_owner ? std::begin(info.queue_infos) + 1 : std::begin(info.queue_infos);
		auto end_it = std::end(info.queue_infos);
		auto min_el = std::min_element(begin_it, end_it, [](const auto& lhs, const auto& rhs) {
			return lhs.pending_tasks > rhs.pending_tasks;
		});
		const std::size_t idx = static_cast<std::size_t>(std::distance(std::begin(info.queue_infos), min_el));
		return idx;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_most_free_queue_idx ()
	/// <summary>
	/// Gets the most free queue idx. This is basicly used by the load balancing
	/// mechanism.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::size_t get_most_free_queue_idx(bool skip_owner) const
	{
		if(_threads_count == 1)
			return get_owner_thread_idx();

		auto info = get_info();
		auto begin_it = skip_owner ? std::begin(info.queue_infos) + 1 : std::begin(info.queue_infos);
		auto end_it = std::end(info.queue_infos);
		auto min_el = std::min_element(begin_it, end_it, [](const auto& lhs, const auto& rhs) {
			return lhs.pending_tasks < rhs.pending_tasks;
		});
		const std::size_t idx = static_cast<std::size_t>(std::distance(std::begin(info.queue_infos), min_el));
		return idx;
	}
	//-----------------------------------------------------------------------------
	//  Name : push_on_thread ()
	/// <summary>
	/// Pushes a task to a specific thread to be executed when it can.
	/// Either a ready task or an awaitable one
	/// </summary>
	//-----------------------------------------------------------------------------
	template <class F, class... Args>
	auto push_on_thread(const std::size_t idx, F&& f, Args&&... args)
	{
		using is_ready_task = nonstd::all_true<!is_future<Args>::value...>;
		return push_impl(is_ready_task(), idx, false, std::forward<F>(f), std::forward<Args>(args)...);
	}

	//-----------------------------------------------------------------------------
	//  Name : push_on_worker_thread ()
	/// <summary>
	/// Pushes a task to a worker thread to be executed when it can.
	/// Either a ready task or an awaitable one
	/// </summary>
	//-----------------------------------------------------------------------------
	template <class F, class... Args>
	auto push_on_worker_thread(F&& f, Args&&... args)
	{
		const std::size_t idx = get_any_worker_thread_idx();
		return push_on_thread(idx, std::forward<F>(f), std::forward<Args>(args)...);
	}

	//-----------------------------------------------------------------------------
	//  Name : push_on_owner_thread ()
	/// <summary>
	/// Pushes a task to the owner thread to be executed when it can.
	/// The owner's thread is the thread that the task_system operates on.
	/// It should be the thread that created the system and that calls
	/// run_on_owner_thread.
	/// Either a ready task or an awaitable one
	/// </summary>
	//-----------------------------------------------------------------------------
	template <class F, class... Args>
	auto push_on_owner_thread(F&& f, Args&&... args)
	{
		const std::size_t idx = get_owner_thread_idx();
		return push_on_thread(idx, std::forward<F>(f), std::forward<Args>(args)...);
	}

	//-----------------------------------------------------------------------------
	//  Name : push_or_execute_on_thread ()
	/// <summary>
	/// Pushes a task to a specific thread to be executed.
	/// If the pusher is in the same thread as the destination it will check if the
	/// task is ready to be executed. If it is ready it will be executed immediately.
	/// Either a ready task or an awaitable one
	/// </summary>
	//-----------------------------------------------------------------------------
	template <class F, class... Args>
	auto push_or_execute_on_thread(const std::size_t idx, F&& f, Args&&... args)
	{
		using is_ready_task = nonstd::all_true<!is_future<Args>::value...>;
		return push_impl(is_ready_task(), idx, true, std::forward<F>(f), std::forward<Args>(args)...);
	}

	//-----------------------------------------------------------------------------
	//  Name : push_or_execute_on_worker_thread ()
	/// <summary>
	/// Pushes a task to a worker thread to be executed.
	/// If the pusher is in the same thread as the destination it will check if the
	/// task is ready to be executed. If it is ready it will be executed immediately.
	/// Either a ready task or an awaitable one
	/// </summary>
	//-----------------------------------------------------------------------------
	template <class F, class... Args>
	auto push_or_execute_on_worker_thread(F&& f, Args&&... args)
	{
		const std::size_t idx = get_any_worker_thread_idx();
		return push_or_execute_on_thread(idx, std::forward<F>(f), std::forward<Args>(args)...);
	}

	//-----------------------------------------------------------------------------
	//  Name : push_or_execute_on_owner_thread ()
	/// <summary>
	/// Pushes a task to the owner thread to be executed. The owner's thread is the
	/// thread that the task_system operates on. It should be the thread that created
	/// the system and that calls run_on_owner_thread.
	/// If the pusher is in the same thread as the destination it will check if the
	/// task is ready to be executed. If it is ready it will be executed immediately.
	/// Either a ready task or an awaitable one
	/// </summary>
	//-----------------------------------------------------------------------------
	template <class F, class... Args>
	auto push_or_execute_on_owner_thread(F&& f, Args&&... args)
	{
		const std::size_t idx = get_owner_thread_idx();
		return push_or_execute_on_thread(idx, std::forward<F>(f), std::forward<Args>(args)...);
	}

private:
	//-----------------------------------------------------------------------------
	//  Name : push_impl ()
	/// <summary>
	/// Pushes a immediately invokable task to be executed.
	/// Ready tasks are assumed to be immediately invokable; that is,
	/// invoking the underlying pakcaged_task with the provided arguments
	/// will not block.This is contrasted with async tasks where some or
	/// all of the provided arguments may be futures waiting on results of
	/// other tasks.
	/// </summary>
	//-----------------------------------------------------------------------------
	template <class F, class... Args>
	auto push_impl(std::true_type, std::size_t idx, bool execute_if_ready, F&& f, Args&&... args)
	{
		return push_task(
			make_ready_task(std::allocator_arg_t{}, _alloc, std::forward<F>(f), std::forward<Args>(args)...),
			idx, execute_if_ready);
	}

	//-----------------------------------------------------------------------------
	//  Name : push_impl ()
	/// <summary>
	/// Pushes an awaitable task to be executed.
	/// Awaitable tasks are assumed to take arguments where some or all are
	/// backed by futures waiting on results of other tasks.This is
	/// contrasted with ready tasks that are assumed to be immediately invokable.
	/// </summary>
	//-----------------------------------------------------------------------------
	template <class F, class... Args>
	auto push_impl(std::false_type, std::size_t idx, bool execute_if_ready, F&& f, Args&&... args)
	{
		return push_task(make_awaitable_task(std::allocator_arg_t{}, _alloc, std::forward<F>(f),
											 std::forward<Args>(args)...),
						 idx, execute_if_ready);
	}

	//-----------------------------------------------------------------------------
	//  Name : push_task ()
	/// <summary>
	/// Pushes a task to be executed.
	/// Ready tasks are assumed to be immediately invokable; that is,
	/// invoking the underlying pakcaged_task with the provided arguments
	/// will not block.This is contrasted with async tasks where some or
	/// all of the provided arguments may be futures waiting on results of
	/// other tasks.
	/// Awaitable tasks are assumed to take arguments where some or all are
	/// backed by futures waiting on results of other tasks.This is
	/// contrasted with ready tasks that are assumed to be immediately invokable.
	/// </summary>
	//-----------------------------------------------------------------------------
	template <class T>
	auto push_task(T&& t, std::size_t idx, bool execute_if_ready) ->
		typename std::remove_reference<decltype(t.second)>::type
	{
		t.second._system = this;

		const auto queue_index = get_thread_queue_idx(idx);
		if(execute_if_ready && t.first.ready() &&
		   ((get_thread_id(queue_index) == std::this_thread::get_id()) || (queue_index != 0)))
		{
			t.first();

			return std::move(t.second);
		}
		else
		{
			_queues[queue_index].push(std::move(t.first));
			return std::move(t.second);
		}
	}

	//-----------------------------------------------------------------------------
	//  Name : processing_wait ()
	/// <summary>
	/// Waits for a task in a smart way. Allows processing of other task on this
	/// thread. The task is guaranteed to be ready when this function exits if
	/// the task is executed by the owner thread or any of the worker threads that
	/// we manage.
	/// </summary>
	//-----------------------------------------------------------------------------
	template <typename T>
	bool processing_wait(const task_future<T>& t)
	{
		using namespace std::literals;
		constexpr std::size_t invalid_index = 77777;

		std::pair<bool, task> p = {false, task()};

		const auto this_thread_id = std::this_thread::get_id();

		std::size_t queue_index = invalid_index;

		for(std::size_t i = 0; i < _threads_count; ++i)
		{
			if(get_thread_id(i) == this_thread_id)
			{
				queue_index = get_thread_queue_idx(i, 0);
				break;
			}
		}

		if(queue_index == invalid_index)
			return false;

		const auto condition = [&t]() { return !t.is_ready(); };

		run(queue_index, condition, 5ms);

		return true;
	}

	//-----------------------------------------------------------------------------
	//  Name : run ()
	/// <summary>
	/// Main loop of our worker threads
	/// </summary>
	//-----------------------------------------------------------------------------
	void run(std::size_t idx, std::function<bool()> condition, duration_t pop_timeout = duration_t::max());

	//-----------------------------------------------------------------------------
	//  Name : get_thread_queue_idx ()
	/// <summary>
	/// Gets the thread's queue index.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::size_t get_thread_queue_idx(std::size_t idx, std::size_t seed = 0);

	//-----------------------------------------------------------------------------
	//  Name : get_thread_id ()
	/// <summary>
	/// Gets the thread's id by thread index.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::thread::id get_thread_id(std::size_t index);

	class task_queue
	{

	public:
		task_queue();
		task_queue(task_queue const&) = delete;
		task_queue(task_queue&& other) noexcept;

		std::size_t get_pending_tasks() const;
		void set_done();
		bool is_done() const;
		std::pair<bool, task> try_pop();
		bool try_push(task& t);
		std::pair<bool, task> pop(duration_t pop_timeout = duration_t::max());

		void push(task t);
		void wake_up();

	private:
		void sort();
		std::deque<task> _tasks;
		std::condition_variable _cv;
		mutable std::mutex _mutex;
		std::atomic_bool _done{false};
	};

	std::atomic<std::uint32_t> _steals = {0};
	std::vector<task_queue> _queues;
	std::vector<std::thread> _threads;
	typename allocator_t::template rebind<task::task_concept>::other _alloc;
	std::size_t _threads_count;
	//
	const std::thread::id _owner_thread_id = std::this_thread::get_id();
};

template <typename T>
void task_future<T>::wait() const
{
	if(!future.valid())
		return;

	if(!is_ready())
	{
		if(_system)
		{
			if(!_system->processing_wait(*this))
			{
				future.wait();
			}
		}
		else
		{
			future.wait();
		}
	}
}
} // namespace core

#endif // #ifndef TASK_SYSTEM_H
