#include "task_system.h"

namespace core
{
awaitable_task::task_concept::~task_concept() noexcept = default;

void task_system::task_queue::rotate_()
{
	/* zero or one element list -- trivial to rotate */
	if(tasks_.empty())
		return;

	std::rotate(tasks_.begin(), tasks_.begin() + 1, tasks_.end());
}

task_system::task_queue::task_queue()
	: tasks_{}
{
}

task_system::task_queue::task_queue(task_system::task_queue&& other) noexcept
	: tasks_(std::move(other).tasks_)
	, done_(other.done_.load())
{
}

bool task_system::task_queue::is_empty()
{
	std::unique_lock<std::mutex> lock(mutex_);
	return tasks_.empty();
}

void task_system::task_queue::set_done()
{
	done_.store(true);
	cv_.notify_all();
}

std::pair<bool, awaitable_task> task_system::task_queue::try_pop()
{
	std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);

	if(!lock || tasks_.empty())
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

bool task_system::task_queue::try_push(awaitable_task& t)
{
	{
		std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);
		if(!lock)
			return false;

		tasks_.emplace_back(std::move(t));
	}

	cv_.notify_one();
	return true;
}

std::pair<bool, awaitable_task> task_system::task_queue::pop(bool wait)
{
	std::unique_lock<std::mutex> lock(mutex_);

	if(wait)
	{
		while(tasks_.empty() && !done_)
		{
			cv_.wait(lock);
		}
	}

	if(tasks_.empty())
		return std::make_pair(false, awaitable_task{});

	auto sz = tasks_.size();
	for(decltype(sz) i = 0; i < sz; ++i)
	{
		if(tasks_.front().ready())
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
							 * If we get to this point the best we can do is pop
	 * from the
							 * front of the task list, release the lock, and wait
	 * for the
							 * task to be ready.
							 */
	if(wait)
	{
		auto t = std::move(tasks_.front());
		tasks_.pop_front();
		lock.unlock();

		while(!t.ready())
			std::this_thread::yield();

		return std::make_pair(true, std::move(t));
	}
	else
	{
		return std::make_pair(false, awaitable_task{});
	}
}

void task_system::task_queue::push(awaitable_task t)
{
	{
		std::unique_lock<std::mutex> lock(mutex_);
		tasks_.emplace_back(std::move(t));
	}
	cv_.notify_one();
}

void task_system::run(std::size_t idx)
{
	while(true)
	{
		std::pair<bool, awaitable_task> p = {false, awaitable_task()};

		for(std::size_t k = 0; k < 10 * nthreads_; ++k)
		{
			const auto queue_index = get_thread_queue_idx(idx, k);
			p = queues_[queue_index].try_pop();
			if(p.first)
				break;
		}

		if(!p.first)
		{
			const auto queue_index = get_thread_queue_idx(idx);
			p = queues_[queue_index].pop();
			if(!p.first)
				return;
		}

		if(p.first)
			p.second();
	}
}

std::size_t task_system::get_thread_queue_idx(std::size_t idx, std::size_t seed)
{
    // if main thread then just return
    if(idx == 0)
        return 0;
    
	auto queue_index = ((idx + seed) % nthreads_);
	if(queue_index == 0)
		queue_index++;

	return queue_index;
}


std::thread::id task_system::get_thread_id(std::size_t index)
{
    const auto& thread = threads_[index];
    
    const auto thread_id = (index == 0) ? detail::get_main_thread_id() : thread.get_id();
    
    return thread_id;
}

task_system::task_system()
    : task_system(std::thread::hardware_concurrency())
{
}

task_system::task_system(std::size_t nthreads, const task_system::Allocator& alloc)
    : queues_{}
    , threads_{}
    , alloc_(alloc)
    , nthreads_{nthreads + 1}
{
    // +1 for the main thread's queue
	queues_.reserve(nthreads_);
	queues_.emplace_back();
	for(std::size_t th = 1; th < nthreads_; ++th)
	{
		queues_.emplace_back();
	}

	// two seperate loops.
	threads_.reserve(nthreads_);
	threads_.emplace_back();
	for(std::size_t th = 1; th < nthreads_; ++th)
	{
		threads_.emplace_back(&task_system::run, this, th);
	}
}

task_system::~task_system()
{
	dispose();
	for(auto& th : threads_)
	{
		if(th.joinable())
			th.join();
	}
}

void core::task_system::dispose()
{
	for(auto& q : queues_)
		q.set_done();
}

void task_system::run_on_main()
{
	std::pair<bool, awaitable_task> p = {false, awaitable_task()};

	const auto queue_index = get_thread_queue_idx(0);

	p = queues_[queue_index].pop(false);
	if(!p.first)
		return;

	if(p.first)
		p.second();
}
}
