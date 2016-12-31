#pragma once

#include "subsystem.h"
#include "common/spin.hpp"
#include "common/handle_object_set.hpp"

#include <vector>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace core
{

	//
	// Why we need a automatic task scheduler:
	// 1. Better scalability, easier to take advantage of N cores by automatic
	// load balancing;
	// 2. Less error-prone, dependencies can be expressed as simple parent-child
	// relationships between tasks;
	// 3. Easier to gain benefits from both function and data parallelism.

	/**
	 * @brief      A Light-weight task scheduler with automatic load balancing,
	 * the dependencies between tasks are addressed as parent-child relationships.
	 */
	struct TaskSystem : public Subsystem
	{
		TaskSystem(unsigned worker = 0) : _core(worker) {}

		// initialize task scheduler with specified worker count
		bool initialize() override;
		// shutdown task scheduler, this would block main thread until all the tasks finished
		void dispose() override;

		// create_task
		Handle create(const char* name);

		template<typename F, typename ... Args>
		Handle create(const char* name, F&& functor, Args&& ... args);

		template<typename F, typename ... Args>
		Handle create_as_child(Handle parent, const char* name, F&& functor, Args&&... args);

		// perform certain task for a fixed number of elements
		template<typename F, typename IT>
		Handle create_parallel_for(const char* name, F&& functor, IT begin, IT end, size_t step);

		// run_task insert a task into a queue instead of executing it immediately
		void run(Handle);

		// run_task insert a task into a queue instead of executing it immediately
		void run_on_main(Handle);

		void execute_tasks_on_main();

		// wait_task
		void wait(Handle);

		// returns true if task completed
		bool is_completed(Handle);

		// returns main thread id
		std::thread::id get_main_thread() const { return _thread_main; }

	protected:
		struct Task
		{
			Task() {}
			Task(Task&& rhs) : jobs(rhs.jobs.load())
			{
				closure = std::move(rhs.closure);
				parent = rhs.parent;
				strncpy(name, rhs.name, strlen(rhs.name));
			}

			std::function<void()> closure = nullptr;
			std::atomic<uint32_t> jobs;
			Handle parent;
			char name[64] = { 0 };
		};

		// create a task
		Handle create_internal(const char*, std::function<void()>);

		// create_task_as_child comes with parent-child relationships:
		// 1. a task should be able to have N child tasks;
		// 2. waiting for a task to be completed must properly synchronize across its children
		// as well
		Handle create_as_child_internal(Handle, const char*, std::function<void()>);

	public:
		// several callbacks instended for thread initialization and profilers
		using thread_callback = std::function<void(unsigned)>;
		thread_callback on_thread_start;
		thread_callback on_thread_stop;
		// several callbacks instended for task based profiling
		using task_callback = std::function<void(unsigned, const char*)>;
		task_callback on_task_start;
		task_callback on_task_stop;

	protected:
		static void thread_run(TaskSystem&, unsigned index);

		void finish(Handle);
		bool execute_one(unsigned, bool, std::mutex&, std::queue<Handle>&);
		unsigned get_thread_index() const;

	protected:
		unsigned _core;

		std::mutex _tasks_mutex;
		DynamicHandleObjectSet<Task, 32> _tasks;

		std::mutex _queue_mutex;
		std::queue<Handle> _alive_tasks;

		std::mutex _main_queue_mutex;
		std::queue<Handle> _main_alive_tasks;

		std::vector<std::thread> _workers;
		std::thread::id _thread_main;
		std::condition_variable _condition;
		bool _stop;

		std::unordered_map<std::thread::id, unsigned> _thread_indices;
	};

	//
	// IMPLEMENTATIONS of JOBSYSTEM
	inline Handle TaskSystem::create(const char* name)
	{
		return create_internal(name, nullptr);
	}

	template<typename F, typename ... Args>
	Handle TaskSystem::create(const char* name, F&& functor, Args&& ... args)
	{
		auto functor_with_env = std::bind(std::forward<F>(functor), std::forward<Args>(args)...);
		return create_internal(name, functor_with_env);
	}

	template<typename F, typename ... Args>
	Handle TaskSystem::create_as_child(Handle parent, const char* name, F&& functor, Args&&... args)
	{
		auto functor_with_env = std::bind(std::forward<F>(functor), std::forward<Args>(args)...);
		return create_as_child_internal(parent, name, functor_with_env);
	}

	template<typename F, typename IT>
	Handle TaskSystem::create_parallel_for(const char* name, F&& functor, IT begin, IT end, size_t step)
	{
		auto master = create_internal(name, nullptr);
		for (auto it = begin; it < end; it += step)
			run(create_as_child_internal(master, name, std::bind(std::forward<F>(functor), it, it + step)));
		return master;
	}

}