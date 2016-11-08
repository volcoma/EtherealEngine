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
	//-----------------------------------------------------------------------------
	//  Name : isReady (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool isReady() const = 0;

	//-----------------------------------------------------------------------------
	//  Name : invokeCallback (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void invokeCallback() = 0;

	//-----------------------------------------------------------------------------
	//  Name : waitUntilReady (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void waitUntilReady() = 0;
};

template<typename ReturnType>
struct TTask : public ITask
{
	//-----------------------------------------------------------------------------
	//  Name : Task ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	explicit TTask(std::shared_future<ReturnType> f)
		: sharedFuture(f)
	{}

	//-----------------------------------------------------------------------------
	//  Name : isReady (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool isReady() const
	{
		return sharedFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
	}

	//-----------------------------------------------------------------------------
	//  Name : waitUntilReady (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void waitUntilReady()
	{
		sharedFuture.get();
	}

	//-----------------------------------------------------------------------------
	//  Name : getResult ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	ReturnType& getResult() const
	{
		return sharedFuture.get();
	}

	//-----------------------------------------------------------------------------
	//  Name : invokeCallback (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void invokeCallback()
	{
	}

	/// shared future holding the result
	std::shared_future<ReturnType> sharedFuture;
};

template<>
struct TTask<void> : public ITask
{
	//-----------------------------------------------------------------------------
	//  Name : Task ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	explicit TTask(std::shared_future<void> f)
		: sharedFuture(f)
	{}

	//-----------------------------------------------------------------------------
	//  Name : isReady (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool isReady() const
	{
		return sharedFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
	}

	//-----------------------------------------------------------------------------
	//  Name : waitUntilReady (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void waitUntilReady()
	{
		sharedFuture.get();
	}

	//-----------------------------------------------------------------------------
	//  Name : getResult ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void getResult() const
	{
		return sharedFuture.get();
	}

	//-----------------------------------------------------------------------------
	//  Name : invokeCallback (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void invokeCallback()
	{
	}
	/// shared future holding the result
	std::shared_future<void> sharedFuture;
};

template<typename ReturnType, typename OnReadyFunc>
struct Task : public TTask<ReturnType>
{
	//-----------------------------------------------------------------------------
	//  Name : Task ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	explicit Task(std::shared_future<ReturnType> f, OnReadyFunc&& readyFunction)
		: TTask(f)
		, onReady(readyFunction)
	{}

	//-----------------------------------------------------------------------------
	//  Name : invokeCallback (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void invokeCallback()
	{
		onReady();
	}
	/// ready callback
	OnReadyFunc onReady;
};

template<typename OnReadyFunc>
struct Task<void, OnReadyFunc> : public TTask<void>
{
	//-----------------------------------------------------------------------------
	//  Name : Task ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	explicit Task(std::shared_future<void> f, OnReadyFunc&& readyFunction)
		: TTask(f)
		, onReady(readyFunction)
	{}

	//-----------------------------------------------------------------------------
	//  Name : invokeCallback (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void invokeCallback()
	{
		onReady();
	}
	/// ready callback
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
	/// need to keep track of threads so we can join them
	std::vector< std::thread > mWorkers;
	/// the task queue
	std::queue< std::function<void()> > mTasks;
	/// the result array
	std::vector<std::shared_ptr<ITask>> mResults;
	/// Mutex for synchronization of the tasks
	std::mutex mTaskMutex;
	/// Condition variable for notifying threads
	std::condition_variable mCondition;
	/// Mutex for synchronization of the results
	std::mutex mResultMutex;
	/// Is pool stopped
	bool mStopped = false;
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
					std::unique_lock<std::mutex> lock(mTaskMutex);
					mCondition.wait(lock,
						[this]()
					{
						return mStopped || !mTasks.empty();
					});
					if (mStopped && mTasks.empty())
						return;
					task = std::move(mTasks.front());
					mTasks.pop();
				}
				task();
			}
		});
		thread_utils::setThreadName(&worker, string_utils::format("Worker_Thread_%d", i));
		mWorkers.emplace_back(std::move(worker));
	}
}

// add new work item to the pool
template<class F, class C, class... Args>
inline auto ThreadPool::enqueue_with_callback(F&& f, C&& callback, Args&&... args)
-> std::shared_ptr<Task<typename std::result_of<F(Args...)>::type, C>>
{
	Expects(!mStopped);

	using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared< std::packaged_task<return_type()> >
		(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);

	std::shared_future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(mTaskMutex);
		mTasks.emplace([task]() { (*task)(); });
	}
	auto sharedFutureWrapper = std::make_shared<Task<return_type, C>>(res, std::forward<C>(callback));
	{
		std::unique_lock<std::mutex> lock(mResultMutex);
		mResults.push_back(sharedFutureWrapper);
	}

	mCondition.notify_one();

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
		std::unique_lock<std::mutex> lock(mTaskMutex);
		mStopped = true;
	}
	mCondition.notify_all();

	for (auto &worker : mWorkers)
		worker.join();

	mWorkers.clear();
}

inline void ThreadPool::poll()
{
	std::vector<std::shared_ptr<ITask>> result_container;
	{
		std::unique_lock<std::mutex> lock(mResultMutex);
		result_container = mResults;
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
			std::unique_lock<std::mutex> lock(mResultMutex);
			mResults.erase(std::remove_if(std::begin(mResults), std::end(mResults),
				[result](std::shared_ptr<ITask> res)
			{
				return res == result;
			}), std::end(mResults));
		}
		result->invokeCallback();
	}
}