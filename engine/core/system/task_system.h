#ifndef TASK_SYSTEM_H
#define TASK_SYSTEM_H

#include "../common/nonstd/function_traits.hpp"
#include "../common/nonstd/sequence.hpp"
#include "../common/nonstd/type_traits.hpp"
#include "subsystem.h"
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

namespace detail
{
template <void (*ctor)()>
struct static_initializer
{
	struct constructor
	{
		constructor()
		{
			ctor();
		}
	};
	static constructor initializer;
};

template <void (*ctor)()>
typename static_initializer<ctor>::constructor static_initializer<ctor>::initializer;

inline std::thread::id get_main_thread_id()
{
	static auto id = std::this_thread::get_id();
	return id;
}
inline void init_main_thread_id()
{
	static auto init = static_initializer<init_main_thread_id>::initializer;
	(void)init;
	get_main_thread_id();
}
inline bool is_main_thread()
{
	return get_main_thread_id() == std::this_thread::get_id();
}
}

template <typename T>
class task_future
{
public:
	std::shared_future<T> future;

	auto get() const -> decltype(future.get())
	{
		wait();

		return future.get();
	}

	auto valid() const -> decltype(future.valid())
	{
		return future.valid();
	}
	bool is_ready() const
	{
		using namespace std::chrono_literals;
		return valid() && wait_for(0s) == std::future_status::ready;
	}

	void wait() const;

	template <class _Rep, class _Per>
	std::future_status wait_for(const std::chrono::duration<_Rep, _Per>& rel_time) const
	{ // wait for duration
		return future.wait_for(rel_time);
	}

	template <class _Clock, class _Dur>
	std::future_status wait_until(const std::chrono::time_point<_Clock, _Dur>& abs_time) const
	{ // wait until time point
		return future.wait_until(abs_time);
	}

private:
	friend class task_system;
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

namespace
{
inline std::uint64_t get_next_id()
{
	static std::atomic<std::uint64_t> counter{0};
	return counter++;
}
}

/*
 * awaitable_task; a type-erased, allocator-aware std::packaged_task that
 * also contains its own arguments. The underlying packaged_task and the
 * stored argument tuple can be heap allocated or allocated with a provided
 * allocator.
 *
 * There are two forms of awaitable_tasks: ready tasks and async tasks.
 *
 *      Ready tasks are assumed to be immediately invokable; that is,
 *      invoking the underlying pakcaged_task with the provided arguments
 *      will not block. This is contrasted with async tasks where some or
 *      all of the provided arguments may be futures waiting on results of
 *      other tasks.
 *
 *      Awaitable tasks are assumed to take arguments where some or all are
 *      backed by futures waiting on results of other tasks. This is
 *      contrasted with ready tasks that are assumed to be immediately
 *      invokable.
 *
 * There are two helper methods for creating awaitable_task objects:
 * make_ready_task and make_awaitable_task, both of which return a pair of
 * the newly constructed awaitable_task and a std::future object to the
 * return value.
 */
class awaitable_task
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
	awaitable_task() = default;
	~awaitable_task() = default;

	awaitable_task(awaitable_task const&) = delete;
	awaitable_task(awaitable_task&&) noexcept = default;

	awaitable_task& operator=(awaitable_task const&) = delete;
	awaitable_task& operator=(awaitable_task&&) noexcept = default;

	void swap(awaitable_task& other) noexcept
	{
		std::swap(_t, other._t);
	}

	operator bool() const noexcept
	{
		return static_cast<bool>(_t);
	}

	std::uint64_t get_id() const
	{
		return _t->id;
	}

	friend class task_system;

	template <class F, class... Args>
	friend std::pair<awaitable_task, task_future<typename std::result_of<F(Args...)>::type>>
	make_ready_task(F&& f, Args&&... args)
	{
		using pair_type = std::pair<awaitable_task, task_future<typename std::result_of<F(Args...)>::type>>;
		using model_type = ready_task_model<typename std::result_of<F(Args...)>::type(Args...)>;

		awaitable_task t(ready_task_tag(), std::forward<F>(f), std::forward<Args>(args)...);
		auto fut = static_cast<model_type&>(*t._t).get_future();
		return pair_type(std::move(t), std::move(fut));
	}

	template <class Allocator, class F, class... Args>
	friend std::pair<awaitable_task, task_future<typename std::result_of<F(Args...)>::type>>
	make_ready_task(std::allocator_arg_t, Allocator const& alloc, F&& f, Args&&... args)
	{
		using pair_type = std::pair<awaitable_task, task_future<typename std::result_of<F(Args...)>::type>>;
		using model_type = ready_task_model<typename std::result_of<F(Args...)>::type(Args...)>;

		awaitable_task t(ready_task_tag(), std::allocator_arg_t(), alloc, std::forward<F>(f),
						 std::forward<Args>(args)...);
		auto fut = static_cast<model_type&>(*t._t).get_future();
		return pair_type(std::move(t), std::move(fut));
	}

	template <class F, class... Args>
	friend std::pair<awaitable_task, task_future<typename std::result_of<F(decay_if_future<Args>...)>::type>>
	make_awaitable_task(F&& f, Args&&... args)
	{
		using pair_type = std::pair<awaitable_task,
									task_future<typename std::result_of<F(decay_if_future<Args>...)>::type>>;
		using model_type = awaitable_task_model<
			typename std::result_of<F(decay_if_future<Args>...)>::type(decay_if_future<Args>...), Args...>;

		awaitable_task t(awaitable_task_tag(), std::forward<F>(f), std::forward<Args>(args)...);
		auto fut = static_cast<model_type&>(*t._t).get_future();
		return pair_type(std::move(t), std::move(fut));
	}

	template <class Allocator, class F, class... Args>
	friend std::pair<awaitable_task, task_future<typename std::result_of<F(decay_if_future<Args>...)>::type>>
	make_awaitable_task(std::allocator_arg_t, Allocator const& alloc, F&& f, Args&&... args)
	{
		using pair_type = std::pair<awaitable_task,
									task_future<typename std::result_of<F(decay_if_future<Args>...)>::type>>;
		using model_type = awaitable_task_model<
			typename std::result_of<F(decay_if_future<Args>...)>::type(decay_if_future<Args>...), Args...>;

		awaitable_task t(awaitable_task_tag(), std::allocator_arg_t(), alloc, std::forward<F>(f),
						 std::forward<Args>(args)...);
		auto fut = static_cast<model_type&>(*t._t).get_future();
		return pair_type(std::move(t), std::move(fut));
	}

	void operator()()
	{
		if(_t)
			_t->invoke_();
		else
			throw std::logic_error("bad task access");
	}

	bool ready() const
	{
		if(_t)
			return _t->ready_();
		else
			throw std::logic_error("bad task access");
	}

private:
	template <class F, class... Args>
	awaitable_task(ready_task_tag, F&& f, Args&&... args)
		: _t(new ready_task_model<typename std::result_of<F(Args...)>::type(Args...)>(
			  std::forward<F>(f), std::forward<Args>(args)...))
	{
	}

	template <class Allocator, class F, class... Args>
	awaitable_task(ready_task_tag, std::allocator_arg_t, Allocator const& alloc, F&& f, Args&&... args)
		: _t(new ready_task_model<typename std::result_of<F(Args...)>::type(Args...)>(
			  std::allocator_arg_t(), alloc, std::forward<F>(f), std::forward<Args>(args)...))
	{
	}

	template <class F, class... Args>
	awaitable_task(awaitable_task_tag, F&& f, Args&&... args)
		: _t(new awaitable_task_model<
			  typename std::result_of<F(decay_if_future<Args>...)>::type(decay_if_future<Args>...), Args...>(
			  std::forward<F>(f), std::forward<Args>(args)...))
	{
	}

	template <class Allocator, class F, class... Args>
	awaitable_task(awaitable_task_tag, std::allocator_arg_t, Allocator const& alloc, F&& f, Args&&... args)
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

		const std::uint64_t id = get_next_id();
	};

	template <class>
	struct ready_task_model;

	/*
	 * Ready tasks are assumed to be immediately invokable; that is,
	 * invoking the underlying pakcaged_task with the provided arguments
	 * will not block. This is contrasted with async tasks where some or all
	 * of the provided arguments may be futures waiting on results of other
	 * tasks.
	 */
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
			task_future<R> result;
			result.future = _f.get_future().share();
			return result;
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

	/*
	 * Async tasks are assumed to take arguments where some or all are
	 * backed by futures waiting on results of other tasks. This is
	 * contrasted with ready tasks that are assumed to be immediately
	 * invokable.
	 */
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
			task_future<R> result;
			result.future = _f.get_future().share();
			return result;
		}

		void invoke_() override
		{
			constexpr const std::size_t arity = sizeof...(FutArgs);
			do_invoke_(nonstd::make_index_sequence<arity>());
		}

		bool ready_() const noexcept override
		{
			constexpr const std::size_t arity = sizeof...(FutArgs);
			return do_ready_(nonstd::make_index_sequence<arity>());
		}

	private:
		template <class T>
		static inline auto call_get(T&& t) noexcept -> decltype(std::forward<T>(t))
		{
			return std::forward<T>(t);
		}

		template <class T>
		static inline auto call_get(task_future<T>&& t) noexcept -> decltype(t.future.get())
		{
			return t.future.get();
		}

		template <class T>
		static inline auto call_get(std::future<T>&& t) noexcept -> decltype(t.get())
		{
			return t.get();
		}

		template <class T>
		static inline auto call_get(std::shared_future<T>&& t) noexcept -> decltype(t.get())
		{
			return t.get();
		}

		template <std::size_t... I>
		inline void do_invoke_(nonstd::index_sequence<I...>)
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
			using namespace std::chrono_literals;
			return t.valid() && t.wait_for(0s) == std::future_status::ready;
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
		inline bool do_ready_(nonstd::index_sequence<I...>) const noexcept
		{
			return nonstd::check_all_true(call_ready(std::get<I>(_args))...);
		}

		std::packaged_task<R(CallArgs...)> _f;
		std::tuple<nonstd::special_decay_t<FutArgs>...> _args;
	};

	std::unique_ptr<task_concept> _t;
};

class task_system : public core::subsystem
{
	using Allocator = std::allocator<awaitable_task>;
	class task_queue
	{
		std::deque<awaitable_task> tasks_;
		std::condition_variable cv_;
		std::mutex mutex_;
		std::atomic_bool done_{false};

		/* rotates the first element of the list onto the end */
		void rotate_();

	public:
		task_queue();
		task_queue(task_queue const&) = delete;
		task_queue(task_queue&& other) noexcept;

		bool is_empty();
		void set_done();
		std::pair<bool, awaitable_task> try_pop();
		bool try_push(awaitable_task& t);
		std::pair<bool, awaitable_task> pop(bool wait = true);
		void push(awaitable_task t);
	};

	std::vector<task_queue> queues_;
	std::vector<std::thread> threads_;
	typename Allocator::template rebind<awaitable_task::task_concept>::other alloc_;
	std::size_t nthreads_;
	std::atomic<std::size_t> current_index_{1};

	void run(std::size_t idx);
	std::size_t get_thread_queue_idx(std::size_t idx, std::size_t seed = 0);
	std::thread::id get_thread_id(std::size_t index);

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

		std::size_t try_count = idx == get_main_thread_idx() ? 0 : 10 * nthreads_;

		for(std::size_t k = 0; k < try_count; ++k)
		{
			const auto queue_index = get_thread_queue_idx(idx, k);

			if(execute_if_ready && t.first.ready() &&
			   ((get_thread_id(queue_index) == std::this_thread::get_id()) || (queue_index != 0)))
			{
				t.first();

				return std::move(t.second);
			}
			else
			{
				if(queues_[queue_index].try_push(t.first))
					return std::move(t.second);
			}
		}

		const auto queue_index = get_thread_queue_idx(idx);
		if(execute_if_ready && t.first.ready() &&
		   ((get_thread_id(queue_index) == std::this_thread::get_id()) || (queue_index != 0)))
		{
			t.first();

			return std::move(t.second);
		}
		else
		{
			queues_[queue_index].push(std::move(t.first));
			return std::move(t.second);
		}
	}

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
			make_ready_task(std::allocator_arg_t{}, alloc_, std::forward<F>(f), std::forward<Args>(args)...),
			idx, execute_if_ready);
	}

	//-----------------------------------------------------------------------------
	//  Name : push_awaitable ()
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
		return push_task(make_awaitable_task(std::allocator_arg_t{}, alloc_, std::forward<F>(f),
											 std::forward<Args>(args)...),
						 idx, execute_if_ready);
	}

public:
	task_system();

	task_system(std::size_t nthreads, Allocator const& alloc = Allocator());

	//-----------------------------------------------------------------------------
	//  Name : ~task_system ()
	/// <summary>
	/// Notifies threads to finish and joins them.
	/// </summary>
	//-----------------------------------------------------------------------------
	~task_system();

	//-----------------------------------------------------------------------------
	//  Name : initialize ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool initialize() override
	{
		return true;
	}

	//-----------------------------------------------------------------------------
	//  Name : dispose ()
	/// <summary>
	/// Notify queues on threads to finish.
	/// </summary>
	//-----------------------------------------------------------------------------
	void dispose() override;

	std::size_t get_main_thread_idx()
	{
		return 0;
	}
	std::size_t get_worker_thread_idx()
	{
		const std::size_t idx = (nthreads_ == 1) ? get_main_thread_idx() : current_index_++;
		return idx;
	}

	//-----------------------------------------------------------------------------
	//  Name : push ()
	/// <summary>
	/// Pushes a task to be executed. Either a ready task or an awaitable one
	/// </summary>
	//-----------------------------------------------------------------------------
    template <class F, class... Args>
	auto push(const std::size_t idx, F&& f, Args&&... args)
	{
		using is_ready_task = nonstd::all_true<!is_future<Args>::value...>;
		return push_impl(is_ready_task(), idx, false, std::forward<F>(f), std::forward<Args>(args)...);
	}
    
	template <class F, class... Args>
	auto push(F&& f, Args&&... args)
	{
		const std::size_t idx = get_worker_thread_idx();
		return push(idx, std::forward<F>(f), std::forward<Args>(args)...);
	}

    template <class F, class... Args>
	auto push_or_execute(const std::size_t idx, F&& f, Args&&... args)
	{
		using is_ready_task = nonstd::all_true<!is_future<Args>::value...>;
		return push_impl(is_ready_task(), idx, true, std::forward<F>(f), std::forward<Args>(args)...);
	}
    
	template <class F, class... Args>
	auto push_or_execute(F&& f, Args&&... args)
	{
		const std::size_t idx = get_worker_thread_idx();
		return push_or_execute(idx, std::forward<F>(f), std::forward<Args>(args)...);
	}

	template <class F, class... Args>
	auto push_on_main(F&& f, Args&&... args)
	{
		const std::size_t idx = get_main_thread_idx();
		return push(idx, std::forward<F>(f), std::forward<Args>(args)...);
	}

	template <class F, class... Args>
	auto push_or_execute_on_main(F&& f, Args&&... args)
	{
        const std::size_t idx = get_main_thread_idx();
		return push_or_execute(idx, std::forward<F>(f), std::forward<Args>(args)...);
	}

	//-----------------------------------------------------------------------------
	//  Name : run_on_main ()
	/// <summary>
	/// Process main thread tasks
	/// </summary>
	//-----------------------------------------------------------------------------
	void run_on_main();

	constexpr static const std::size_t invalid_index = 77777;

	template <typename T>
	bool processing_wait(const task_future<T>& task)
	{
		std::pair<bool, awaitable_task> p = {false, awaitable_task()};

		const auto this_thread_id = std::this_thread::get_id();

		std::size_t queue_index = invalid_index;

		for(std::size_t i = 0; i < nthreads_; ++i)
		{
			if(get_thread_id(i) == this_thread_id)
			{
				queue_index = get_thread_queue_idx(i, 0);
				break;
			}
		}

		if(queue_index == invalid_index)
			return false;

		auto& queue = queues_[queue_index];

        while(!task.is_ready())
        {
            bool became_ready = false;
            while(!queue.is_empty())
            {
                p = queue.pop(queue_index != get_main_thread_idx());
                if(!p.first)
                    continue;
    
                if(p.first)
                    p.second();
    
                became_ready = task.is_ready();
                if(became_ready)
                    break;
            }
            using namespace std::literals;
            if(!became_ready)
                task.wait_for(5ms);
            else
                break;
        }
		return true;
	}
};

template <typename T>
void task_future<T>::wait() const
{
	if(!is_ready())
	{
		if(_system)
		{
			if(!_system->processing_wait(*this))
			{
				if(future.valid())
				{
					future.wait();
				}
			}
		}
		else if(future.valid())
		{
			future.wait();
		}
	}
}
} // namespace core

#endif // #ifndef TASK_SYSTEM_H
