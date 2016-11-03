#pragma once
#include "thread_utils.h"
#include "Core/common/string_utils.h"
#include "Core/internal/assert/assert.h"

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

struct ITask
{
	virtual ~ITask() = default;
	virtual bool isReady() const = 0;
	virtual void invokeCallback() = 0;
	virtual void waitUntilReady() = 0;
};

template<typename ReturnType, typename OnReadyFunc>
struct Task : public ITask
{
	explicit Task(std::shared_future<ReturnType> f, OnReadyFunc&& readyFunction)
		: sharedFuture(f)
		, onReady(readyFunction)
	{}

	virtual bool isReady() const
	{
		return sharedFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
	}

	virtual void invokeCallback()
	{
		onReady();
	}

	virtual void waitUntilReady()
	{
		sharedFuture.get();
	}

	ReturnType& getResult() const
	{
		return sharedFuture.get();
	}

	std::shared_future<ReturnType> sharedFuture;
	OnReadyFunc onReady;
};

template<typename OnReadyFunc>
struct Task<void, OnReadyFunc> : public ITask
{
	explicit Task(std::shared_future<void> f, OnReadyFunc&& readyFunction)
		: sharedFuture(f)
		, onReady(readyFunction)
	{}
	virtual bool isReady() const
	{
		return sharedFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
	}

	virtual void invokeCallback()
	{
		onReady();
	}

	virtual void waitUntilReady()
	{
		sharedFuture.get();
	}

	void getResult() const
	{
		return sharedFuture.get();
	}

	std::shared_future<void> sharedFuture;
	OnReadyFunc onReady;
};

class ThreadPool
{
public:
    //-----------------------------------------------------------------------------
    //  Name : ThreadPool ()
    /// <summary>
    /// 
    /// 
    /// 
    /// </summary>
    //-----------------------------------------------------------------------------
    ThreadPool(unsigned int threads = std::thread::hardware_concurrency());

	//-----------------------------------------------------------------------------
	//  Name : ~ThreadPool ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~ThreadPool();

    //-----------------------------------------------------------------------------
    //  Name : enqueue_with_callback ()
    /// <summary>
    /// 
    /// 
    /// 
    /// </summary>
    //-----------------------------------------------------------------------------
    template<class F, class C, class... Args>
    auto enqueue_with_callback(F&& f, C&& c, Args&&... args) 
        ->std::shared_ptr<Task<typename std::result_of<F(Args...)>::type, C>>;

	//-----------------------------------------------------------------------------
	//  Name : poll ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void poll();

	//-----------------------------------------------------------------------------
	//  Name : poll ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void poll(std::shared_ptr<ITask> result);

	//-----------------------------------------------------------------------------
	//  Name : shutdown ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void shutdown();
private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // the task queue
    std::queue< std::function<void()> > tasks;
	// the result array
	std::vector<std::shared_ptr<ITask>> results;
    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
	std::mutex result_mutex;

    bool stopped = false;
};
 
// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(unsigned int threads)
{
	for (unsigned int i = 0; i < threads; ++i)
	{
		auto worker = std::thread([this]
		{
			std::function<void()> task;
			for (;;)
			{
				{
					std::unique_lock<std::mutex> lock(queue_mutex);
					condition.wait(lock,
						[this] ()
					{
						return stopped || !tasks.empty(); 
					});
					if (stopped && tasks.empty())
						return;
					task = std::move(tasks.front());
					tasks.pop();
				}
				task();
			}
		});
		thread_utils::setThreadName(&worker, string_utils::format("Worker_Thread_%d", i));
        workers.emplace_back(std::move(worker));
	}
}


// add new work item to the pool
template<class F, class C, class... Args>
inline auto ThreadPool::enqueue_with_callback(F&& f, C&& callback, Args&&... args)
    -> std::shared_ptr<Task<typename std::result_of<F(Args...)>::type, C>>
{
	Expects(!stopped);

    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >
		(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
    std::shared_future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace([task](){ (*task)(); });
    }
	auto sharedFutureWrapper = std::make_shared<Task<return_type, C>>(res, std::forward<C>(callback));
	{
		std::unique_lock<std::mutex> lock(result_mutex);
		results.push_back(sharedFutureWrapper);
	}

    condition.notify_one();

    return sharedFutureWrapper;
}

inline ThreadPool::~ThreadPool()
{
	shutdown();
}

// the destructor joins all threads
inline void ThreadPool::shutdown()
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stopped = true;
	}
	condition.notify_all();

	for (auto &worker : workers)
		worker.join();

	workers.clear();
}

inline void ThreadPool::poll()
{
	std::vector<std::shared_ptr<ITask>> result_container;
	{
		std::unique_lock<std::mutex> lock(result_mutex);
		result_container = results;
	}

	for (auto result : result_container)
	{
		poll(result);
	}
}

inline void ThreadPool::poll(std::shared_ptr<ITask> result)
{
	if (result->isReady())
	{
		{
			std::unique_lock<std::mutex> lock(result_mutex);
			results.erase(std::remove_if(std::begin(results), std::end(results),
				[result](std::shared_ptr<ITask> res)
			{
				return res == result;
			}), std::end(results));
		}

		result->invokeCallback();
	}
}