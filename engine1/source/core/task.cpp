#include "task.h"
#include "common/assert.hpp"

namespace core
{

	bool TaskSystem::initialize()
	{
		if (_core == 0)
			_core = std::thread::hardware_concurrency() - 1;

		_core = std::max(_core, (uint32_t)1);
		_stop = false;
		for (uint32_t i = 0; i < _core; i++)
		{
			_workers.emplace_back(thread_run, std::ref(*this), i + 1);
			_thread_indices.insert(std::make_pair(_workers.back().get_id(), i + 1));
		}

		_thread_main = std::this_thread::get_id();
		_thread_indices.insert(std::make_pair(_thread_main, 0));
		return true;
	}

	void TaskSystem::dispose()
	{
		{
			std::unique_lock<std::mutex> lock(_queue_mutex);
			_stop = true;
		}

		_condition.notify_all();
		for (auto& thread : _workers)
			thread.join();
	}

	Handle TaskSystem::create_internal(const char* name, std::function<void()> closure)
	{
		Handle handle;
		{
			std::unique_lock<std::mutex> L(_tasks_mutex);
			handle = _tasks.create();
		}
		if (handle)
		{
			Task* task = nullptr;
			{
				std::unique_lock<std::mutex> L(_tasks_mutex);
				task = _tasks.fetch(handle);
			}
			if (task)
			{
				task->closure = closure;
				task->jobs.store(1);
				strncpy(task->name, name, std::min(sizeof(task->name), strlen(name)));
			}

			return handle;
		}

		return Handle();
	}

	Handle TaskSystem::create_as_child_internal(Handle parent, const char* name, std::function<void()> closure)
	{
		Handle handle;
		{
			std::unique_lock<std::mutex> L(_tasks_mutex);
			handle = _tasks.create();
		}
		if (handle)
		{
			Task* task = nullptr;
			{
				std::unique_lock<std::mutex> L(_tasks_mutex);
				task = _tasks.fetch(handle);
			}
			if (task)
			{
				task->closure = closure;
				task->jobs.store(1);
				strncpy(task->name, name, std::min(sizeof(task->name), strlen(name)));
			}
			Task* ptask = nullptr;
			{
				std::unique_lock<std::mutex> L(_tasks_mutex);
				ptask = _tasks.fetch(parent);
			}
			if (ptask != nullptr)
			{
				uint32_t current_jobs = ptask->jobs++;
				if (current_jobs > 0) task->parent = parent;
				else ptask->jobs--;
			}
			return handle;
		}
		return Handle();
	}

	void TaskSystem::run(Handle handle)
	{
		Task* task = nullptr;
		{
			std::unique_lock<std::mutex> L(_tasks_mutex);
			task = _tasks.fetch(handle);
		}
		Expects(task != nullptr && task->jobs.load() > 0);
		//"invalid task handle to run.");

		{
			std::unique_lock<std::mutex> L(_queue_mutex);
			_alive_tasks.push(handle);
		}

		_condition.notify_one();
	}

	void TaskSystem::run_on_main(Handle handle)
	{
		Task* task = nullptr;
		{
			std::unique_lock<std::mutex> L(_tasks_mutex);
			task = _tasks.fetch(handle);
		}
		Expects(task != nullptr && task->jobs.load() > 0);
		//"invalid task handle to run.");

		{
			std::unique_lock<std::mutex> L(_main_queue_mutex);
			_main_alive_tasks.push(handle);
		}
		unsigned index = get_thread_index();
		if (index == 0)
			execute_one(index, true, _main_queue_mutex, _main_alive_tasks);
	}

	void TaskSystem::execute_tasks_on_main()
	{
		unsigned index = get_thread_index();
		while (!_main_alive_tasks.empty())
		{
			execute_one(index, true, _main_queue_mutex, _main_alive_tasks);
		}
	}

	bool TaskSystem::is_completed(Handle handle)
	{
		Task* task = nullptr;
		{
			std::unique_lock<std::mutex> L(_tasks_mutex);
			task = _tasks.fetch(handle);
		}
		if (task == nullptr)
			return true;
		return task->jobs.load() == 0;
	}

	void TaskSystem::wait(Handle handle)
	{
		Task* task = nullptr;
		{
			std::unique_lock<std::mutex> L(_tasks_mutex);
			task = _tasks.fetch(handle);
		}
		if (task == nullptr)
			return;

		unsigned index = get_thread_index();
		while (!is_completed(handle))
		{
			std::this_thread::yield();
			if (!execute_one(index, false, _queue_mutex, _alive_tasks))
				break;
		}
	}

	void TaskSystem::finish(Handle handle)
	{
		Task* task = nullptr;
		{
			std::unique_lock<std::mutex> L(_tasks_mutex);
			task = _tasks.fetch(handle);
		}
		if (task == nullptr)
			return;

		// atomic decrement
		const uint32_t jobs = --task->jobs;
		if (jobs == 0)
		{
			finish(task->parent);

			// free captured reference and recycle task
			task->closure = nullptr;
			task->parent.invalidate(); // invalidate parent handle
			{
				std::unique_lock<std::mutex> L(_tasks_mutex);
				_tasks.free(handle);
			}
		}
	}

	bool TaskSystem::execute_one(unsigned index, bool wait, std::mutex& mtx, std::queue<Handle>& queue)
	{
		Handle handle;

		{
			std::unique_lock<std::mutex> L(mtx);
			if (wait)
				_condition.wait(L, [this, &queue] { return _stop || !queue.empty(); });

			if (_stop && queue.empty())
				return false;

			if (!wait && queue.empty())
				return true;

			handle = queue.front();
			queue.pop();
		}

		if (auto task = _tasks.fetch(handle))
		{
			if (on_task_start)
				on_task_start(index, task->name);

			if (task->closure != nullptr)
				task->closure();

			finish(handle);

			if (on_task_stop)
				on_task_stop(index, task->name);
		}

		return true;
	}

	unsigned TaskSystem::get_thread_index() const
	{
		auto found = _thread_indices.find(std::this_thread::get_id());
		if (found != _thread_indices.end()) return found->second;
		return 0xFFFFFFFF;
	}

	void TaskSystem::thread_run(TaskSystem& scheduler, unsigned index)
	{
		if (scheduler.on_thread_start)
			scheduler.on_thread_start(index);

		for (;; )
		{
			if (!scheduler.execute_one(index, true, scheduler._queue_mutex, scheduler._alive_tasks))
				break;
		}

		if (scheduler.on_thread_stop)
			scheduler.on_thread_stop(index);
	}

}