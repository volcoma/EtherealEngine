#pragma once
#include <memory>
#include <string>

#include "core/task.h"
#include "core/events/event.hpp"
#include "asset_handle.h"

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
	void then(const delegate<void(AssetHandle<T>)>& callback)
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
		if (loadTask.is_valid())
		{
			auto ts = core::get_subsystem<core::TaskSystem>();
			ts->wait(loadTask);
			ts->execute_tasks_on_main();
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
	void setTask(core::Handle task)
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
	core::Handle loadTask;
	/// Subscribed callbacks
	event<void(AssetHandle<T>)> callbacks;
};