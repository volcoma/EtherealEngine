#pragma once
#include <memory>
#include <string>
#include <functional>
#include "Core/events/event.hpp"
#include "AssetHandle.h"
#include "../System/Application.h"
#include "../Threading/ThreadPool.h"

template<typename T>
struct LoadRequest
{
	//-----------------------------------------------------------------------------
	//  Name : then ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void then(std::function<void(AssetHandle<T>)> callback)
	{
		if (isReady())
			callback(asset);
		else
			callbacks.addListener(callback);
	}

	//-----------------------------------------------------------------------------
	//  Name : isReady ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isReady() const
	{
		return !!asset.link->asset;
	}

	//-----------------------------------------------------------------------------
	//  Name : waitUntilReady ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void waitUntilReady()
	{
		if (loadTask)
		{
			loadTask->waitUntilReady();
			auto& app = Singleton<Application>::getInstance();
			auto& threadPool = app.getThreadPool();
			threadPool.poll(loadTask);
		}
	}

	//-----------------------------------------------------------------------------
	//  Name : setTask ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setTask(std::shared_ptr<ITask> task)
	{
		loadTask = task;
	}

	//-----------------------------------------------------------------------------
	//  Name : setData ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setData(const std::string& id, std::shared_ptr<T> data)
	{
		asset.link->id = id;
		asset.link->asset = data;
	}

	//-----------------------------------------------------------------------------
	//  Name : invokeCallbacks ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void invokeCallbacks()
	{
		callbacks(asset);
		callbacks = {};
	}

	/// Requested asset
	AssetHandle<T> asset;
	/// Associated task with this request
	std::shared_ptr<ITask> loadTask;
	/// Subscribed callbacks
	event<void(AssetHandle<T>)> callbacks;
};