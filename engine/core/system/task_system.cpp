#include "task_system.h"

namespace core
{

task::task_concept::~task_concept() noexcept = default;

void task_system::task_queue::rotate_()
{
	/* zero or one element list -- trivial to rotate */
	if(_tasks.empty())
		return;

	std::rotate(_tasks.begin(), _tasks.begin() + 1, _tasks.end());
}

task_system::task_queue::task_queue()
	: _tasks{}
{
}

task_system::task_queue::task_queue(task_system::task_queue&& other) noexcept
	: _tasks(std::move(other)._tasks)
	, _done(other._done.load())
{
}

bool task_system::task_queue::is_empty()
{
	std::unique_lock<std::mutex> lock(_mutex);
	return _tasks.empty();
}

void task_system::task_queue::set_done()
{
	_done.store(true);
	_cv.notify_all();
}

std::pair<bool, task> task_system::task_queue::try_pop()
{
	std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);

	if(!lock || _tasks.empty())
	{
		return std::make_pair(false, task{});
	}
	else if(_tasks.front().ready())
	{
		auto t = std::move(_tasks.front());
		_tasks.pop_front();
		return std::make_pair(true, std::move(t));
	}

	rotate_();
	return std::make_pair(false, task{});
}

bool task_system::task_queue::try_push(task& t)
{
	{
		std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
		if(!lock)
			return false;

		_tasks.emplace_back(std::move(t));
	}

	_cv.notify_one();
	return true;
}

std::pair<bool, task> task_system::task_queue::pop(bool wait)
{
	std::unique_lock<std::mutex> lock(_mutex);

	if(wait)
	{
		while(_tasks.empty() && !_done)
		{
			_cv.wait(lock);
		}
	}

	if(_tasks.empty())
		return std::make_pair(false, task{});

	auto sz = _tasks.size();
	for(decltype(sz) i = 0; i < sz; ++i)
	{
		if(_tasks.front().ready())
		{
			auto t = std::move(_tasks.front());
			_tasks.pop_front();
			return std::make_pair(true, std::move(t));
		}
		else
		{
			rotate_();
		}
	}

	// If we get to this point the best we can do is pop
	// from the
	// front of the task list, release the lock, and wait
	// for the
	// task to be ready.

	if(wait)
	{
		auto t = std::move(_tasks.front());
		_tasks.pop_front();
		lock.unlock();

		while(!t.ready())
			std::this_thread::yield();

		return std::make_pair(true, std::move(t));
	}
	else
	{
		return std::make_pair(false, task{});
	}
}

void task_system::task_queue::push(task t)
{
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_tasks.emplace_back(std::move(t));
	}
	_cv.notify_one();
}

void task_system::run(std::size_t idx)
{
	while(true)
	{
		std::pair<bool, task> p = {false, task()};

		for(std::size_t k = 0; k < 10 * _threads_count; ++k)
		{
			const auto queue_index = get_thread_queue_idx(idx, k);
			p = _queues[queue_index].try_pop();
			if(p.first)
				break;
		}

		if(!p.first)
		{
			const auto queue_index = get_thread_queue_idx(idx);
			p = _queues[queue_index].pop();
			if(!p.first)
				return;
		}

		if(p.first)
			p.second();
	}
}

std::size_t task_system::get_thread_queue_idx(std::size_t idx, std::size_t seed)
{
	// if owner thread then just return
	if(idx == get_owner_thread_idx())
		return get_owner_thread_idx();

	auto queue_index = ((idx + seed) % _threads_count);

	if(queue_index == get_owner_thread_idx())
		queue_index++;

	return queue_index;
}

std::thread::id task_system::get_thread_id(std::size_t index)
{
	const auto& thread = _threads[index];

	const auto thread_id = (index == get_owner_thread_idx()) ? _owner_thread_id : thread.get_id();

	return thread_id;
}

task_system::task_system()
	: task_system(std::thread::hardware_concurrency() - 1)
{
}

task_system::task_system(std::size_t nthreads, const task_system::Allocator& alloc)
	: _alloc(alloc)
	, _threads_count{nthreads + 1}
{
	// +1 for the owner thread's queue
	_queues.reserve(_threads_count);
	_queues.emplace_back();
	for(std::size_t th = 1; th < _threads_count; ++th)
	{
		_queues.emplace_back();
	}

	// two seperate loops.
	_threads.reserve(_threads_count);
	_threads.emplace_back();
	for(std::size_t th = 1; th < _threads_count; ++th)
	{
		_threads.emplace_back(&task_system::run, this, th);
	}
}

task_system::~task_system()
{
	dispose();
	for(auto& th : _threads)
	{
		if(th.joinable())
			th.join();
	}
}

void core::task_system::dispose()
{
	for(auto& q : _queues)
		q.set_done();
}

void task_system::run_on_owner_thread()
{
	std::pair<bool, task> p = {false, task()};

	const auto queue_index = get_thread_queue_idx(0);

	p = _queues[queue_index].pop(false);
	if(!p.first)
		return;

	if(p.first)
		p.second();
}
}
