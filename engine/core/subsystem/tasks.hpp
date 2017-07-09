#ifndef AWAITABLE_TASK_HPP
#define AWAITABLE_TASK_HPP

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <exception>
#include <forward_list>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include "../common/nonstd/function_traits.hpp"
#include "../common/nonstd/sequence.hpp"
#include "../common/nonstd/type_traits.hpp"
#include "subsystem.h"

namespace core
{
	class task_system;

	namespace detail
	{
		template<void(*ctor)()>
		struct static_initializer
		{
			struct constructor { constructor() { ctor(); } };
			static constructor initializer;
		};

		template<void(*ctor)()>
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

	template<typename T>
	class task_future
	{
	public:
		std::shared_future<T> future;
		std::uint64_t id = 0;		
	
		auto get() const -> decltype (future.get())
		{
			wait();

			return future.get();
		}

		auto valid() const -> decltype (future.valid())
		{
			return future.valid();
		}
		bool is_ready() const
		{
			using namespace std::chrono_literals;
			return valid() && wait_for(0s) == std::future_status::ready;
		}

		void wait() const;

		template<class _Rep,
			class _Per>
			std::future_status wait_for(
				const std::chrono::duration<_Rep, _Per>& rel_time) const
		{	// wait for duration
			return future.wait_for(rel_time);
		}

		template<class _Clock,
			class _Dur>
			std::future_status wait_until(
				const std::chrono::time_point<_Clock, _Dur>& abs_time) const
		{	// wait until time point
			return future.wait_until(abs_time);
		}

	private:
		friend class task_system;
		task_system* _system = nullptr;
	};

	template <class>
	struct is_future : std::false_type {};

	template <class T>
	struct is_future <std::future<T>> : std::true_type {};

	template <class T>
	struct is_future <std::shared_future<T>> : std::true_type {};
	template <class T>
	struct is_future <std::shared_future<T>&> : std::true_type {};
	template <class T>
	struct is_future <const std::shared_future<T>&> : std::true_type {};

	template <class T>
	struct is_future <task_future <T>> : std::true_type {};
	template <class T>
	struct is_future <task_future <T>&> : std::true_type {};
	template <class T>
	struct is_future <const task_future <T>&> : std::true_type {};


	template <class T>
	struct decay_future
	{
		using type = T;
	};

	template <class T>
	struct decay_future <std::future <T>>
	{
		using type = T;
	};

	template <class T>
	struct decay_future <std::shared_future <T>>
	{
		using type = T;
	};

	template <class T>
	struct decay_future <std::shared_future<T>&>
	{
		using type = T;
	};

	template <class T>
	struct decay_future <const std::shared_future<T>&>
	{
		using type = T;
	};

	template <class T>
	struct decay_future <task_future <T>>
	{
		using type = T;
	};

	template <class T>
	struct decay_future <task_future<T>&>
	{
		using type = T;
	};

	template <class T>
	struct decay_future <const task_future<T>&>
	{
		using type = T;
	};

	namespace
	{
		inline std::uint64_t get_next_id()
		{
			static std::atomic<std::uint64_t> counter{ 0 };
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
		using decay_if_future = typename std::conditional <
			is_future <typename std::decay <T>::type>::value,
			typename decay_future <T>::type, T
		>::type;

		struct ready_task_tag {};
		struct awaitable_task_tag {};

	public:
		awaitable_task() = default;
		~awaitable_task() = default;

		awaitable_task(awaitable_task const &) = delete;
		awaitable_task(awaitable_task &&) noexcept = default;

		awaitable_task & operator= (awaitable_task const &) = delete;
		awaitable_task & operator= (awaitable_task &&) noexcept = default;

		void swap(awaitable_task & other) noexcept
		{
			std::swap(_t, other._t);
		}

		operator bool() const noexcept
		{
			return static_cast <bool> (_t);
		}

		std::uint64_t get_id() const
		{
			return _t->id;
		}

		friend class task_system;

		template <class F, class ... Args>
		friend std::pair <
			awaitable_task,
			task_future <typename std::result_of <F(Args...)>::type>
		> make_ready_task(F && f, Args && ... args)
		{
			using pair_type = std::pair <
				awaitable_task,
				task_future <typename std::result_of <F(Args...)>::type>
			>;
			using model_type = ready_task_model <
				typename std::result_of <F(Args...)>::type(Args...)
			>;

			awaitable_task t(
				ready_task_tag(),
				std::forward <F>(f), std::forward <Args>(args)...
			);
			auto fut = static_cast <model_type &> (*t._t).get_future();
			return pair_type(std::move(t), std::move(fut));
		}

		template <class Allocator, class F, class ... Args>
		friend std::pair <
			awaitable_task,
			task_future <typename std::result_of <F(Args...)>::type>
		> make_ready_task(std::allocator_arg_t, Allocator const & alloc,
			F && f, Args && ... args)
		{
			using pair_type = std::pair <
				awaitable_task,
				task_future <typename std::result_of <F(Args...)>::type>
			>;
			using model_type = ready_task_model <
				typename std::result_of <F(Args...)>::type(Args...)
			>;

			awaitable_task t(
				ready_task_tag(), std::allocator_arg_t(), alloc,
				std::forward <F>(f), std::forward <Args>(args)...
			);
			auto fut = static_cast <model_type &> (*t._t).get_future();
			return pair_type(std::move(t), std::move(fut));
		}

		template <class F, class ... Args>
		friend std::pair <
			awaitable_task,
			task_future <typename std::result_of <
			F(decay_if_future <Args>...)
			>::type>
		> make_awaitable_task(F && f, Args && ... args)
		{
			using pair_type = std::pair <
				awaitable_task,
				task_future <typename std::result_of <
				F(decay_if_future <Args>...)
				>::type>
			>;
			using model_type = awaitable_task_model <
				typename std::result_of <
				F(decay_if_future <Args>...)
				>::type(decay_if_future <Args>...),
				Args...
			>;

			awaitable_task t(
				awaitable_task_tag(),
				std::forward <F>(f), std::forward <Args>(args)...
			);
			auto fut = static_cast <model_type &> (*t._t).get_future();
			return pair_type(std::move(t), std::move(fut));
		}

		template <class Allocator, class F, class ... Args>
		friend std::pair <
			awaitable_task,
			task_future <typename std::result_of <
			F(decay_if_future <Args>...)
			>::type>
		> make_awaitable_task(std::allocator_arg_t, Allocator const & alloc,
			F && f, Args && ... args)
		{
			using pair_type = std::pair <
				awaitable_task,
				task_future <typename std::result_of <
				F(decay_if_future <Args>...)
				>::type>
			>;
			using model_type = awaitable_task_model <
				typename std::result_of <
				F(decay_if_future <Args>...)
				>::type(decay_if_future <Args>...),
				Args...
			>;

			awaitable_task t(
				awaitable_task_tag(), std::allocator_arg_t(), alloc,
				std::forward <F>(f), std::forward <Args>(args)...
			);
			auto fut = static_cast <model_type &> (*t._t).get_future();
			return pair_type(std::move(t), std::move(fut));
		}

		void operator() ()
		{
			if (_t)
				_t->invoke_();
			else
				throw std::logic_error("bad task access");
		}

		bool ready() const
		{
			if (_t)
				return _t->ready_();
			else
				throw std::logic_error("bad task access");
		}

	private:
		template <class F, class ... Args>
		awaitable_task(ready_task_tag, F && f, Args && ... args)
			: _t(new ready_task_model <
				typename std::result_of <F(Args...)>::type(Args...)
			>(std::forward <F>(f), std::forward <Args>(args)...)
			)
		{}

		template <class Allocator, class F, class ... Args>
		awaitable_task(ready_task_tag,
			std::allocator_arg_t,
			Allocator const & alloc,
			F && f, Args && ... args)
			: _t(
				new ready_task_model <
				typename std::result_of <F(Args...)>::type(Args...)
				>(std::allocator_arg_t(), alloc,
					std::forward <F>(f), std::forward <Args>(args)...)
			)
		{
		}

		template <class F, class ... Args>
		awaitable_task(awaitable_task_tag, F && f, Args && ... args)
			: _t(
				new awaitable_task_model <
				typename std::result_of <
				F(decay_if_future <Args>...)
				>::type(decay_if_future <Args>...),
				Args...
				>(std::forward <F>(f), std::forward <Args>(args)...)
			)
		{}

		template <class Allocator, class F, class ... Args>
		awaitable_task(awaitable_task_tag,
			std::allocator_arg_t,
			Allocator const & alloc,
			F && f, Args && ... args)
			: _t(
				new awaitable_task_model <
				typename std::result_of <
				F(decay_if_future <Args>...)
				>::type(decay_if_future <Args>...),
				Args...
				>(std::allocator_arg_t(), alloc,
					std::forward <F>(f), std::forward <Args>(args)...)
			)
		{}

		struct task_concept
		{
			virtual ~task_concept() noexcept {}
			virtual void invoke_() = 0;
			virtual bool ready_() const noexcept = 0;

			std::uint64_t id = get_next_id();
		};

		template <class> struct ready_task_model;

		/*
		 * Ready tasks are assumed to be immediately invokable; that is,
		 * invoking the underlying pakcaged_task with the provided arguments
		 * will not block. This is contrasted with async tasks where some or all
		 * of the provided arguments may be futures waiting on results of other
		 * tasks.
		 */
		template <class R, class ... Args>
		struct ready_task_model <R(Args...)> : task_concept
		{
			template <class F>
			explicit ready_task_model(F && f, Args && ... args)
				: _f(std::forward <F>(f))
				, _args(std::forward <Args>(args)...)
			{}

			template <class Allocator, class F>
			explicit ready_task_model(
				std::allocator_arg_t, Allocator const & alloc,
				F && f, Args && ... args
			)
				: _f(std::allocator_arg_t(), alloc, std::forward <F>(f))
                , _args(std::forward<Args>(args)...)
			{}

			task_future<R> get_future()
			{
				task_future<R> result;
				result.future = _f.get_future().share();
				result.id = id;
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
			
			std::packaged_task <R(Args...)> _f;
			std::tuple <nonstd::special_decay_t<Args>...> _args;		
		};

		template <class ...> struct awaitable_task_model;

		/*
		 * Async tasks are assumed to take arguments where some or all are
		 * backed by futures waiting on results of other tasks. This is
		 * contrasted with ready tasks that are assumed to be immediately
		 * invokable.
		 */
		template <class R, class ... CallArgs, class ... FutArgs>
		struct awaitable_task_model <R(CallArgs...), FutArgs...> : task_concept
		{
			template <class F, class ... Args>
			explicit awaitable_task_model(F && f, Args && ... args)
				: _f(std::forward <F>(f))
				, _args(std::forward <Args>(args)...)
			{}

			template <class Allocator, class F, class ... Args>
			explicit awaitable_task_model(
				std::allocator_arg_t, Allocator const & alloc,
				F && f, Args && ... args
			)
				: _f(std::allocator_arg_t(), alloc, std::forward <F>(f))
				, _args(std::forward <Args>(args)...)
			{}

			task_future<R> get_future()
			{
				task_future<R> result;
				result.future = _f.get_future().share();
				result.id = id;
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
			static inline auto call_get(T && t) noexcept
				-> decltype (std::forward <T>(t))
			{
				return std::forward <T>(t);
			}

			template <class T>
			static inline auto call_get(task_future<T> && t) noexcept
				-> decltype (t.future.get())
			{
				return t.future.get();
			}

			template <class T>
			static inline auto call_get(std::future<T> && t) noexcept
				-> decltype (t.get())
			{
				return t.get();
			}

			template <class T>
			static inline auto call_get(std::shared_future<T> && t) noexcept
				-> decltype (t.get())
			{
				return t.get();
			}

			template <std::size_t ... I>
			inline void do_invoke_(nonstd::index_sequence <I...>)
			{
				//_f(call_get(std::get <I>(std::move(_args)))...);
				nonstd::invoke(_f, call_get(std::get <I>(std::move(_args)))...);
			}

			template <class T>
			static inline bool call_ready(const T &) noexcept { return true; }

			template <class T>
			static inline bool call_ready(const task_future<T> & t) noexcept
			{
				using namespace std::chrono_literals;
				return t.wait_for(0s) == std::future_status::ready;
			}

			template <class T>
			static inline bool call_ready(const std::future<T> & t) noexcept
			{
				using namespace std::chrono_literals;
				return t.wait_for(0s) == std::future_status::ready;
			}

			template <class T>
			static inline bool call_ready(const std::shared_future<T> & t) noexcept
			{
				using namespace std::chrono_literals;
				return t.wait_for(0s) == std::future_status::ready;
			}

			template <std::size_t ... I>
			inline bool do_ready_(nonstd::index_sequence <I...>) const noexcept
			{
				return nonstd::check_all_true(call_ready(std::get<I>(_args))...);
			}

			std::packaged_task <R(CallArgs...)> _f;
			std::tuple <nonstd::special_decay_t<FutArgs>...> _args;			
		};

		std::unique_ptr <task_concept> _t;
	};


	class task_system : public core::subsystem
	{
		using Allocator = std::allocator <awaitable_task>;
		class task_queue
		{
			//using iterator_type =
			//	typename std::forward_list <awaitable_task>::iterator;

			//std::forward_list <awaitable_task> tasks_;
			//iterator_type last_;
			std::deque<awaitable_task> tasks_;
			std::condition_variable cv_;
			std::mutex mutex_;
			std::atomic_bool done_{ false };

			/* rotates the first element of the list onto the end */
			void rotate_()
			{
				/* zero or one element list -- trivial to rotate */
				if (tasks_.empty())
					return;

				std::rotate(tasks_.begin(), tasks_.begin() + 1, tasks_.end());
			}

		public:
			task_queue()
				: tasks_{}
			{}

			task_queue(task_queue const &) = delete;

			task_queue(task_queue && other) noexcept
				: tasks_(std::move(other).tasks_)
				, done_(other.done_.load())
			{}

			bool is_empty()
			{
				std::unique_lock <std::mutex> lock(mutex_);
				return tasks_.empty();
			}

			void set_done()
			{
				done_.store(true);
				cv_.notify_all();
			}

			std::pair <bool, awaitable_task> try_pop()
			{
				std::unique_lock <std::mutex>lock(mutex_, std::try_to_lock);

				if (!lock || tasks_.empty())
				{
					return std::make_pair(false, awaitable_task{});
				}
				else if(tasks_.front().ready())
				{
					auto t = std::move(tasks_.front());
					tasks_.pop_front();
					return std::make_pair(true, std::move(t));
				}

				rotate_();
				return std::make_pair(false, awaitable_task{});
			}

			bool try_push(awaitable_task & t)
			{
				{
					std::unique_lock <std::mutex> lock(mutex_, std::try_to_lock);
					if (!lock)
						return false;

					tasks_.emplace_back(std::move(t));
				}

				cv_.notify_one();
				return true;
			}

			std::pair <bool, awaitable_task> pop(bool wait = true)
			{
				std::unique_lock <std::mutex> lock(mutex_);

				if (wait)
				{
					while (tasks_.empty() && !done_)
					{
						cv_.wait(lock);
					}
				}

				if (tasks_.empty())
					return std::make_pair(false, awaitable_task{});

				auto sz = tasks_.size();
				for (decltype(sz) i = 0; i < sz; ++i)
				{
					if (tasks_.front().ready())
					{
						auto t = std::move(tasks_.front());
						tasks_.pop_front();
						return std::make_pair(true, std::move(t));
					}
					else
					{
						rotate_();
					}
				}
				
				/*
				 * If we get to this point the best we can do is pop from the
				 * front of the task list, release the lock, and wait for the
				 * task to be ready.
				 */
				if (wait)
				{
					auto t = std::move(tasks_.front());
					tasks_.pop_front();
					lock.unlock();

					while (!t.ready())
						std::this_thread::yield();

					return std::make_pair(true, std::move(t));
				}
				else
				{
					return std::make_pair(false, awaitable_task{});
				}
			}

			void push(awaitable_task t)
			{
				{
					std::unique_lock <std::mutex> lock(mutex_);
					tasks_.emplace_back(std::move(t));
				}
				cv_.notify_one();
			}
		};

		std::vector <task_queue> queues_;
		std::vector <std::thread> threads_;
		typename Allocator::template rebind <awaitable_task::task_concept>::other
			alloc_;
		std::size_t nthreads_;
		std::size_t current_index_{ 1 };

		void run(std::size_t idx)
		{
			while (true)
			{
				std::pair <bool, awaitable_task> p = { false, awaitable_task() };

				for (std::size_t k = 0; k < 10 * nthreads_; ++k)
				{
					const auto queue_index = get_thread_queue_idx(idx, k);
					p = queues_[queue_index].try_pop();
					if (p.first)
						break;
				}

				if (!p.first)
				{
					p = queues_[idx].pop();
					if (!p.first)
						return;
				}

				if(p.first)
					p.second();
			}
		}

		size_t get_thread_queue_idx(size_t idx, size_t seed = 0)
		{
			return ((idx + seed) % nthreads_) + 1;
		}

		size_t get_main_thread_queue_idx()
		{
			return 0;
		}

	public:
		task_system()
			: task_system(std::thread::hardware_concurrency())
		{}

		task_system(std::size_t nthreads,
			Allocator const & alloc = Allocator())
			: queues_{}
			, threads_{}
			, alloc_(alloc)
			, nthreads_{ nthreads }
		{
			// +1 for the main thread's queue
			queues_.reserve(nthreads + 1);
			queues_.emplace_back();
			for (std::size_t th = 1; th < nthreads + 1; ++th)
				queues_.emplace_back();

			threads_.reserve(nthreads);
			for (std::size_t th = 1; th < nthreads + 1; ++th)
				threads_.emplace_back(
					&task_system::run, this, th
				);
		}

		//-----------------------------------------------------------------------------
		//  Name : ~task_system ()
		/// <summary>
		/// Notifies threads to finish and joins them.
		/// </summary>
		//-----------------------------------------------------------------------------
		~task_system()
		{
			dispose();
			for (auto & th : threads_)
			{
				if (th.joinable())
					th.join();
			}
		}

		//-----------------------------------------------------------------------------
		//  Name : initialize ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool initialize() override
		{
			return true;
		}

		//-----------------------------------------------------------------------------
		//  Name : dispose ()
		/// <summary>
		/// Notify queues on threads to finish.
		/// </summary>
		//-----------------------------------------------------------------------------
		void dispose() override
		{
			for (auto & q : queues_)
				q.set_done();
		}


		//-----------------------------------------------------------------------------
		//  Name : push_ready ()
		/// <summary>
		/// Pushes a immediately invokable task to be executed.
		/// Ready tasks are assumed to be immediately invokable; that is,
		/// invoking the underlying pakcaged_task with the provided arguments
		/// will not block.This is contrasted with async tasks where some or
		/// all of the provided arguments may be futures waiting on results of
		/// other tasks.
		/// </summary>
		//-----------------------------------------------------------------------------
		template <class F, class ... Args>
		auto push_ready(F && f, Args && ... args)
			-> typename std::remove_reference <
			decltype (make_ready_task(
				std::allocator_arg_t{}, alloc_,
				std::forward <F>(f), std::forward <Args>(args)...
			).second)
			> ::type
		{
			if (nthreads_ == 0)
			{
				return push_ready_on_main(std::forward <F>(f), std::forward <Args>(args)...);
			}
			else
			{
				auto t = make_ready_task(
					std::allocator_arg_t{}, alloc_,
					std::forward <F>(f), std::forward <Args>(args)...
				);
				t.second._system = this;
				auto const idx = current_index_++;
				for (std::size_t k = 0; k < 10 * nthreads_; ++k)
				{
					const auto queue_index = get_thread_queue_idx(idx, k);
					if (queues_[queue_index].try_push(t.first))
						return std::move(t.second);
				}

				const auto queue_index = get_thread_queue_idx(idx);
				queues_[queue_index].push(std::move(t.first));
				return std::move(t.second);
			}
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
		template <class F, class ... Args>
		auto push_awaitable(F && f, Args && ... args)
			-> typename std::remove_reference <
			decltype (make_awaitable_task(
				std::allocator_arg_t{}, alloc_,
				std::forward <F>(f), std::forward <Args>(args)...
			).second)
			> ::type
		{
			if (nthreads_ == 0)
			{
				return push_awaitable_on_main(std::forward <F>(f), std::forward <Args>(args)...);
			}
			else
			{
				auto t = make_awaitable_task(
					std::allocator_arg_t{}, alloc_,
					std::forward <F>(f), std::forward <Args>(args)...
				);
				t.second._system = this;
				auto const idx = current_index_++;
				for (std::size_t k = 0; k < 10 * nthreads_; ++k)
				{
					const auto queue_index = get_thread_queue_idx(idx, k);
					if (queues_[queue_index].try_push(t.first))
						return std::move(t.second);
				}

				const auto queue_index = get_thread_queue_idx(idx);
				queues_[queue_index].push(std::move(t.first));
				return std::move(t.second);
			}

		}

		//-----------------------------------------------------------------------------
		//  Name : push_ready_on_main ()
		/// <summary>
		/// Pushes a immediately invokable task to be executed.
		/// Ready tasks are assumed to be immediately invokable; that is,
		/// invoking the underlying pakcaged_task with the provided arguments
		/// will not block.This is contrasted with async tasks where some or
		/// all of the provided arguments may be futures waiting on results of
		/// other tasks.
		/// </summary>
		//-----------------------------------------------------------------------------
		template <class F, class ... Args>
		auto push_ready_on_main(F && f, Args && ... args)
			-> typename std::remove_reference <
			decltype (make_ready_task(
				std::allocator_arg_t{}, alloc_,
				std::forward <F>(f), std::forward <Args>(args)...
			).second)
			> ::type
		{
			auto t = make_ready_task(
				std::allocator_arg_t{}, alloc_,
				std::forward <F>(f), std::forward <Args>(args)...
			);
			t.second._system = this;

			if (detail::is_main_thread() && t.first.ready())
			{
				t.first();

				return std::move(t.second);
			}
			else
			{
				const auto queue_index = get_main_thread_queue_idx();
				for (std::size_t k = 0; k < 10 * nthreads_; ++k)
				{
					if (queues_[queue_index].try_push(t.first))
						return std::move(t.second);
				}

				queues_[queue_index].push(std::move(t.first));
				return std::move(t.second);
			}		
		}


		//-----------------------------------------------------------------------------
		//  Name : push_awaitable_on_main ()
		/// <summary>
		/// Pushes an awaitable task to be executed.
		/// Awaitable tasks are assumed to take arguments where some or all are
		/// backed by futures waiting on results of other tasks.This is
		/// contrasted with ready tasks that are assumed to be immediately invokable.
		/// </summary>
		//-----------------------------------------------------------------------------
		template <class F, class ... Args>
		auto push_awaitable_on_main(F && f, Args && ... args)
			-> typename std::remove_reference <
			decltype (make_awaitable_task(
				std::allocator_arg_t{}, alloc_,
				std::forward <F>(f), std::forward <Args>(args)...
			).second)
			> ::type
		{
			auto t = make_awaitable_task(
				std::allocator_arg_t{}, alloc_,
				std::forward <F>(f), std::forward <Args>(args)...
			);
			t.second._system = this;

			if (detail::is_main_thread() && t.first.ready())
			{
				t.first();
			
				return std::move(t.second);
			}
			else
			{

				const auto queue_index = get_main_thread_queue_idx();
				for (std::size_t k = 0; k < 10; ++k)
				{
					if (queues_[queue_index].try_push(t.first))
						return std::move(t.second);
				}

				queues_[queue_index].push(std::move(t.first));
				return std::move(t.second);
			}
		}

		//-----------------------------------------------------------------------------
		//  Name : run_on_main ()
		/// <summary>
		/// Process main thread tasks
		/// </summary>
		//-----------------------------------------------------------------------------
		void run_on_main()
		{
			std::pair <bool, awaitable_task> p = { false, awaitable_task() };

			const auto queue_index = get_main_thread_queue_idx();
			for (std::size_t k = 0; k < 10; ++k)
			{
				p = queues_[queue_index].try_pop();
				if (p.first)
					break;
			}

			if (!p.first)
			{
				p = queues_[queue_index].pop(false);
				if (!p.first)
					return;
			}

			if (p.first)
				p.second();

		}

		void processing_wait_id_on_main(const std::uint64_t& id)
		{
			std::pair <bool, awaitable_task> p = { false, awaitable_task() };

			const auto queue_index = get_main_thread_queue_idx();

			auto& queue = queues_[queue_index];

			while (!queue.is_empty())
			{
				for (std::size_t k = 0; k < 10; ++k)
				{
					p = queue.try_pop();
					if (p.first)
						break;
				}

				if (!p.first)
				{
					p = queue.pop(false);
					if (!p.first)
						continue;
				}

				if (p.first)
					p.second();

				if (p.second.get_id() == id)
					break;
			}
		}

		template<typename T>
		void processing_wait_on_main(const task_future<T>& task)
		{
			processing_wait_id_on_main(task.id);
		}
		
	};

	template<typename T>
	void task_future<T>::wait() const
	{
		if (!is_ready())
		{
			if (detail::is_main_thread() && _system)
			{
				_system->processing_wait_id_on_main(id);
			}
			else if (future.valid())
			{
				future.wait();
			}
		}
	}
}   // namespace core

#endif  // #ifndef AWAITABLE_TASK_HPP
