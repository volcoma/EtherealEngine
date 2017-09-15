#include "task_system.h"
#include <limits>
namespace core
{

task::task_concept::~task_concept() noexcept = default;

void task_system::task_queue::sort()
{
	if(_tasks.size() > 1)
	{
		std::stable_partition(_tasks.begin(), _tasks.end(), [](const auto& task1) { return task1.ready(); });
	}
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

std::size_t task_system::task_queue::get_pending_tasks() const
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _tasks.size();
}

void task_system::task_queue::set_done()
{
	_done.store(true);
	_cv.notify_all();
}

bool task_system::task_queue::is_done() const
{
	return _done.load();
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

	sort();
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

std::pair<bool, task> task_system::task_queue::pop(std::chrono::milliseconds pop_timeout)
{
	std::unique_lock<std::mutex> lock(_mutex);
	bool wait = pop_timeout > decltype(pop_timeout)(0);
	bool timed_wait = pop_timeout != decltype(pop_timeout)::max();
	if(wait && _tasks.empty())
	{
		if(timed_wait)
		{
			_cv.wait_for(lock, pop_timeout);
		}
		else
		{
			_cv.wait(lock);
		}
	}

	if(_tasks.empty())
		return std::make_pair(false, task{});

	if(_tasks.front().ready())
	{
		auto t = std::move(_tasks.front());
		_tasks.pop_front();
		return std::make_pair(true, std::move(t));
	}
	else
	{
		sort();
	}

	// try after sort
	if(_tasks.front().ready())
	{
		auto t = std::move(_tasks.front());
		_tasks.pop_front();
		return std::make_pair(true, std::move(t));
	}

	return std::make_pair(false, task{});
}

void task_system::task_queue::push(task t)
{
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_tasks.emplace_back(std::move(t));
	}
	_cv.notify_one();
}

void task_system::run(std::size_t idx, std::function<bool()> condition, std::chrono::milliseconds pop_timeout)
{
	while(condition())
	{
		const auto queue_index = get_thread_queue_idx(idx);
		bool is_done = _queues[queue_index].is_done();
		if(is_done && _queues[queue_index].get_pending_tasks() == 0)
			return;

		std::pair<bool, task> p = {false, task()};

		if(idx != 0)
		{
			for(std::size_t k = 0; k < 10 * _threads_count; ++k)
			{
				const auto queue_idx = get_thread_queue_idx(idx, k);
				p = _queues[queue_idx].try_pop();
				if(p.first)
					break;
			}
		}

		if(!p.first)
			p = _queues[queue_index].pop(pop_timeout);

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
	using namespace std::literals;
	for(std::size_t th = 1; th < _threads_count; ++th)
	{
		_threads.emplace_back(&task_system::run, this, th, []() { return true; }, 50ms);
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

	using namespace std::literals;
	p = _queues[queue_index].pop(0ms);
	if(!p.first)
		return;

	if(p.first)
		p.second();
}

task_system::system_info task_system::get_info() const
{
	system_info info;
	info.queue_infos.reserve(_queues.size());
	for(const auto& queue : _queues)
	{
		queue_info q_info;
		q_info.pending_tasks = queue.get_pending_tasks();
		info.pending_tasks += q_info.pending_tasks;
		info.queue_infos.emplace_back(std::move(q_info));
	}
	return info;
}
}
