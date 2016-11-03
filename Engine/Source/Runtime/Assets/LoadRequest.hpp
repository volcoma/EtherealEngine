#pragma once
#include <memory>
#include <string>
#include <functional>
#include "Core/events/event.hpp"
#include "AssetHandle.h"
#include "../Application/Application.h"
#include "../Threading/ThreadPool.h"

template<typename T>
struct LoadRequest
{
	void then(std::function<void(AssetHandle<T>)> callback)
	{
		if (isReady())
			callback(asset);
		else
			callbacks.addListener(callback);
	}

	bool isReady() const
	{
		return !!asset.link->asset;
	}

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
	
	void setTask(std::shared_ptr<ITask> task)
	{
		loadTask = task;
	}

	void setData(const std::string& id, std::shared_ptr<T> data)
	{
		asset.link->id = id;
		asset.link->asset = data;
	}

	void invokeCallbacks()
	{
		callbacks(asset);
		callbacks = {};
	}

	AssetHandle<T> asset;
	event<void(AssetHandle<T>)> callbacks;
	std::shared_ptr<ITask> loadTask;
};