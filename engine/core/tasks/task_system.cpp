#include "task_system.h"
#include "../common/platform/thread.hpp"
#include <limits>

namespace core
{

task::task_concept::~task_concept() noexcept = default;

task::task_concept::task_concept() noexcept
{
	static std::atomic<std::uint64_t> id = {1};
	id_ = id++;
}

void task_system::task_queue::sort()
{
	if(tasks_.size() > 1)
	{
		std::stable_partition(tasks_.begin(), tasks_.end(), [](const auto& task1) { return task1.ready(); });
	}
}

task_system::task_queue::task_queue(task_system::task_queue&& other) noexcept
	: tasks_(std::move(other.tasks_))
	, done_(other.done_.load())
{
}

std::size_t task_system::task_queue::get_pending_tasks() const
{
	std::lock_guard<std::mutex> lock(mutex_);
	return tasks_.size();
}

void task_system::task_queue::clear()
{
	std::unique_lock<std::mutex> lock(mutex_);
	tasks_.clear();
}

void task_system::task_queue::set_done()
{
	done_.store(true);
	cv_.notify_all();
}

bool task_system::task_queue::is_done() const
{
	return done_.load();
}

std::pair<bool, task> task_system::task_queue::try_pop()
{
	std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);

	if(!lock || tasks_.empty())
	{
		return std::make_pair(false, task{});
	}
	if(tasks_.front().ready())
	{
		auto t = std::move(tasks_.front());
		tasks_.pop_front();
		return std::make_pair(true, std::move(t));
	}

	sort();
	return std::make_pair(false, task{});
}

bool task_system::task_queue::try_push(task& t)
{
	{
		std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);
		if(!lock)
		{
			return false;
		}

		tasks_.emplace_back(std::move(t));
	}

	cv_.notify_one();
	return true;
}

std::pair<bool, task> task_system::task_queue::pop(duration_t pop_timeout)
{
	std::unique_lock<std::mutex> lock(mutex_);
	bool wait = pop_timeout > decltype(pop_timeout)(0);
	bool timed_wait = pop_timeout != decltype(pop_timeout)::max();
	if(wait && tasks_.empty())
	{
		if(timed_wait)
		{
			cv_.wait_for(lock, pop_timeout);
		}
		else
		{
			cv_.wait(lock);
		}
	}

	if(tasks_.empty())
	{
		return std::make_pair(false, task{});
	}

	if(tasks_.front().ready())
	{
		auto t = std::move(tasks_.front());
		tasks_.pop_front();
		return std::make_pair(true, std::move(t));
	}

	sort();

	// try after sort
	if(tasks_.front().ready())
	{
		auto t = std::move(tasks_.front());
		tasks_.pop_front();
		return std::make_pair(true, std::move(t));
	}

	return std::make_pair(false, task{});
}

void task_system::task_queue::push(task t)
{
	{
		std::unique_lock<std::mutex> lock(mutex_);
		tasks_.emplace_back(std::move(t));
	}
	cv_.notify_one();
}

void task_system::task_queue::wake_up()
{
	cv_.notify_all();
}

bool task_system::task_queue::cancel(uint64_t id)
{
	bool res = false;
	{
		std::unique_lock<std::mutex> lock(mutex_);
		tasks_.erase(std::remove_if(std::begin(tasks_), std::end(tasks_),
									[id, &res](const auto& task) {
										auto cmp = task.get_id() == id;
										if(cmp)
										{
											res = true;
										}
										return cmp;
									}),
					 std::end(tasks_));
	}
	cv_.notify_one();

	return res;
}

void task_system::run(std::size_t idx, const std::function<bool()>& condition, duration_t pop_timeout)
{
	while(condition())
	{
		const auto queue_index = get_thread_queue_idx(idx);
		bool is_done = queues_[queue_index].is_done();
		bool is_empty = queues_[queue_index].get_pending_tasks() == 0;
		if(is_done && is_empty)
		{
			return;
		}

		std::pair<bool, task> p = {false, task()};

		if(idx != 0 && is_empty)
		{
			std::size_t steal_attempts = threads_count_;
			const auto queue_idx = get_most_free_queue_idx(true);
			for(std::size_t k = 0; k < steal_attempts; ++k)
			{
				if(queue_index != queue_idx)
				{
					p = queues_[queue_idx].try_pop();
					if(p.first)
					{
						break;
					}
				}
			}
		}

		if(!p.first)
		{
			p = queues_[queue_index].pop(pop_timeout);
		}

		if(p.first)
		{
			p.second();
		}
	}
}

std::size_t task_system::get_thread_queue_idx(std::size_t idx, std::size_t seed)
{
	// if owner thread then just return
	if(idx == get_owner_thread_idx())
	{
		return get_owner_thread_idx();
	}

	auto queue_index = ((idx + seed) % threads_count_);

	if(queue_index == get_owner_thread_idx())
	{
		queue_index++;
	}

	return queue_index;
}

std::thread::id task_system::get_thread_id(std::size_t index)
{
	const auto& thread = threads_[index];

	const auto thread_id = (index == get_owner_thread_idx()) ? owner_thread_id_ : thread.get_id();

	return thread_id;
}

task_system::task_system(bool wait_on_destruct)
	: task_system(wait_on_destruct, std::thread::hardware_concurrency())
{
}

task_system::task_system(bool wait_on_destruct, std::size_t nthreads, const task_system::allocator_t& alloc)
	: alloc_(alloc)
	, threads_count_{nthreads}
	, wait_on_destruct_(wait_on_destruct)
{
	queues_.reserve(threads_count_);
	queues_.emplace_back();
	for(std::size_t th = 1; th < threads_count_; ++th)
	{
		queues_.emplace_back();
	}

	// two seperate loops.
	threads_.reserve(threads_count_);
	threads_.emplace_back();
	using namespace std::literals;
	for(std::size_t th = 1; th < threads_count_; ++th)
	{
		threads_.emplace_back(&task_system::run, this, th, []() { return true; }, 50ms);
		platform::set_thread_name(threads_.back(), "task_worker");
	}
}

task_system::~task_system()
{
	for(auto& q : queues_)
	{
		if(!wait_on_destruct_)
		{
			q.clear();
		}
		q.set_done();
	}

	for(auto& th : threads_)
	{
		if(th.joinable())
		{
			th.join();
		}
	}
}

void task_system::run_on_owner_thread(duration_t max_duration)
{
	const auto queue_index = get_thread_queue_idx(0);

	using namespace std::literals;
	auto now = std::chrono::steady_clock::now();
	auto end = now + max_duration;

	while(now < end)
	{
		auto p = queues_[queue_index].pop(0ms);
		if(!p.first)
		{
			return;
		}

		if(p.first)
		{
			p.second();
		}

		now = std::chrono::steady_clock::now();
	}
}

task_system::system_info task_system::get_info() const
{
	system_info info;
	info.queue_infos.reserve(queues_.size());
	for(const auto& queue : queues_)
	{
		queue_info q_info;
		q_info.pending_tasks = queue.get_pending_tasks();
		info.pending_tasks += q_info.pending_tasks;
		info.queue_infos.emplace_back(std::move(q_info));
	}
	return info;
}
}
