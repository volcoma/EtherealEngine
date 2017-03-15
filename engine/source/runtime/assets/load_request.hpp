#pragma once
#include <memory>
#include <string>

#include "../system/task.h"
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
		if (is_ready())
			callback(asset);
		else
			callbacks.connect(callback);
	}

	//-----------------------------------------------------------------------------
	//  Name : is_ready ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool is_ready() const
	{
		return !!asset.link->asset;
	}

	//-----------------------------------------------------------------------------
	//  Name : wait_until_ready ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void wait_until_ready()
	{
		if (load_task.is_valid())
		{
			auto ts = core::get_subsystem<runtime::TaskSystem>();
			ts->wait(load_task);
			// all resource loading tasks create a subtask on the main thread.
			// this way we can be avoid circle wait.
			ts->execute_tasks_on_main({});
		}
	}

	//-----------------------------------------------------------------------------
	//  Name : set_task ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_task(core::Handle task)
	{
		load_task = task;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_data ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_data(const std::string& id, std::shared_ptr<T> data)
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
	void invoke_callbacks()
	{
		callbacks(asset);
		callbacks = {};
	}

	/// Requested asset
	AssetHandle<T> asset;
	/// Associated task with this request
	core::Handle load_task;
	/// Subscribed callbacks
	event<void(AssetHandle<T>)> callbacks;
};