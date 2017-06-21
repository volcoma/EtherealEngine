#pragma once

#include "core/subsystem/subsystem.h"
#include "core/handle/handle_object_set.hpp"

#include <vector>
#include <deque>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace runtime
{

	//
	// Why we need a automatic task scheduler:
	// 1. Better scalability, easier to take advantage of N cores by automatic
	// load balancing;
	// 2. Less error-prone, dependencies can be expressed as simple parent-child
	// relationships between tasks;
	// 3. Easier to gain benefits from both function and data parallelism.

	//-----------------------------------------------------------------------------
	// Main Class Declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : task_system (Class)
	/// <summary>
	/// A Light-weight task scheduler with automatic load balancing,
	/// the dependencies between tasks are addressed as parent - child relationships.
	/// </summary>
	//-----------------------------------------------------------------------------
	struct task_system : public core::subsystem
	{
		task_system(unsigned worker = 0) : _core(worker) {}

		//-----------------------------------------------------------------------------
		//  Name : initialize ()
		/// <summary>
		/// Initialize task task_system with specified worker count
		/// </summary>
		//-----------------------------------------------------------------------------
		bool initialize() override;


		//-----------------------------------------------------------------------------
		//  Name : dispose ()
		/// <summary>
		/// Shutdown task scheduler, this would block main thread until all 
		/// the tasks finished.
		/// </summary>
		//-----------------------------------------------------------------------------
		void dispose() override;

		//-----------------------------------------------------------------------------
		//  Name : create ()
		/// <summary>
		/// Creates task
		/// </summary>
		//-----------------------------------------------------------------------------
		core::handle create(const std::string& name);

		//-----------------------------------------------------------------------------
		//  Name : create ()
		/// <summary>
		/// Creates task
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename F, typename ... Args>
		core::handle create(const std::string& name, F&& functor, Args&& ... args);

		//-----------------------------------------------------------------------------
		//  Name : create_as_child ()
		/// <summary>
		/// Creates task
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename F, typename ... Args>
		core::handle create_as_child(core::handle parent, const std::string& name, F&& functor, Args&&... args);

		//-----------------------------------------------------------------------------
		//  Name : create_parallel_for ()
		/// <summary>
		/// Perform certain task for a fixed number of elements
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename F, typename IT>
		core::handle create_parallel_for(const std::string& name, F&& functor, IT begin, IT end, size_t step);

		//-----------------------------------------------------------------------------
		//  Name : run ()
		/// <summary>
		/// Insert a task into a queue instead of executing it immediately.
		/// If on_main_thread is set to true the task will be executed on the main thread.
		/// </summary>
		//-----------------------------------------------------------------------------
		void run(core::handle, bool on_main_thread = false);

		//-----------------------------------------------------------------------------
		//  Name : execute_tasks_on_main ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void execute_tasks_on_main(std::chrono::duration<float>);

		//-----------------------------------------------------------------------------
		//  Name : wait ()
		/// <summary>
		/// Wait for a task to complete. This will block the current thread.
		/// </summary>
		//-----------------------------------------------------------------------------
		void wait(core::handle);

		//-----------------------------------------------------------------------------
		//  Name : is_completed ()
		/// <summary>
		/// Returns true if task completed.
		/// </summary>
		//-----------------------------------------------------------------------------
		bool is_completed(core::handle);

		//-----------------------------------------------------------------------------
		//  Name : get_main_thread ()
		/// <summary>
		/// Returns main thread id.
		/// </summary>
		//-----------------------------------------------------------------------------
		std::thread::id get_main_thread() const { return _thread_main; }

	protected:
		struct task_t
		{
			task_t() {}
			task_t(task_t&& rhs) : jobs(rhs.jobs.load())
			{
				closure = std::move(rhs.closure);
				parent = rhs.parent;
				name = rhs.name;
			}

			std::function<void()> closure = nullptr;
			std::atomic<uint32_t> jobs;
			core::handle parent;
			std::string name;
		};

		//-----------------------------------------------------------------------------
		//  Name : create_internal ()
		/// <summary>
		/// Creates a task.
		/// </summary>
		//-----------------------------------------------------------------------------
		core::handle create_internal(const std::string&, std::function<void()>);


		//-----------------------------------------------------------------------------
		//  Name : create_as_child_internal ()
		/// <summary>
		/// create_as_child_internal comes with parent-child relationships:
		/// 1. A task should be able to have N child tasks;
		/// 2. Waiting for a task to be completed must properly synchronize across its children
		/// as well.
		/// </summary>
		//-----------------------------------------------------------------------------
		core::handle create_as_child_internal(core::handle, const std::string&, std::function<void()>);

	public:
		/// several callbacks instended for thread initialization and profilers
		using thread_callback = std::function<void(unsigned)>;
		thread_callback on_thread_start;
		thread_callback on_thread_stop;
		/// several callbacks instended for task based profiling
		using task_callback = std::function<void(unsigned, const std::string&)>;
		task_callback on_task_start;
		task_callback on_task_stop;

	protected:
		//-----------------------------------------------------------------------------
		//  Name : thread_run ()
		/// <summary>
		/// Worker threads run method.
		/// </summary>
		//-----------------------------------------------------------------------------
		static void thread_run(task_system&, unsigned index);

		//-----------------------------------------------------------------------------
		//  Name : finish ()
		/// <summary>
		/// Finishes a task.
		/// </summary>
		//-----------------------------------------------------------------------------
		void finish(core::handle);

		//-----------------------------------------------------------------------------
		//  Name : execute_one ()
		/// <summary>
		/// Executes a single task from a queue.
		/// </summary>
		//-----------------------------------------------------------------------------
		bool execute_one(unsigned, bool, std::mutex&, std::deque<core::handle>&);

		//-----------------------------------------------------------------------------
		//  Name : execute_one ()
		/// <summary>
		/// Executes a single task from a queue.
		/// </summary>
		//-----------------------------------------------------------------------------
		bool execute_one(core::handle handle, unsigned, bool, std::mutex&, std::deque<core::handle>&);

		//-----------------------------------------------------------------------------
		//  Name : get_thread_index ()
		/// <summary>
		/// Returns this thread index. Not to be confused with std::this_thread::get_id()
		/// </summary>
		//-----------------------------------------------------------------------------
		unsigned get_thread_index() const;

	protected:
		struct TasksWrapper
		{
			///
			std::mutex mutex;
			///
			std::deque<core::handle> tasks;
		};

		///
		unsigned int _core;
		///
		std::mutex _tasks_mutex;
		///
        core::dynamic_handle_object_set<task_t, 32> _tasks;
		///
		TasksWrapper _main_thread_tasks;
		///
		TasksWrapper _other_thread_tasks;
		///
		std::vector<std::thread> _workers;
		///
		std::thread::id _thread_main;
		///
		std::condition_variable _condition;
		///
		bool _stop;
		///
		std::unordered_map<std::thread::id, unsigned> _thread_indices;
	};

	// IMPLEMENTATIONS of TASKSYSTEM
	inline core::handle task_system::create(const std::string& name)
	{
		return create_internal(name, nullptr);
	}

	template<typename F, typename ... Args>
	core::handle task_system::create(const std::string& name, F&& functor, Args&& ... args)
	{
		auto functor_with_env = std::bind(std::forward<F>(functor), std::forward<Args>(args)...);
		return create_internal(name, functor_with_env);
	}

	template<typename F, typename ... Args>
	core::handle task_system::create_as_child(core::handle parent, const std::string& name, F&& functor, Args&&... args)
	{
		auto functor_with_env = std::bind(std::forward<F>(functor), std::forward<Args>(args)...);
		return create_as_child_internal(parent, name, functor_with_env);
	}

	template<typename F, typename IT>
	core::handle task_system::create_parallel_for(const std::string& name, F&& functor, IT begin, IT end, size_t step)
	{
		auto master = create_internal(name, nullptr);
		for (auto it = begin; it < end; it += step)
			run(create_as_child_internal(master, name, std::bind(std::forward<F>(functor), it, it + step)));
		return master;
	}

}
