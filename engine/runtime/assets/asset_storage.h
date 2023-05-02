#pragma once

#include <functional>
#include <unordered_map>

#include <core/common/hpp/type_index.hpp>
#include <core/string_utils/string_utils.h>
#include <core/tasks/task_system.h>

#include "asset_handle.h"
#include <cassert>

namespace runtime
{

struct basic_storage
{
	//-----------------------------------------------------------------------------
	//  Name : ~base_storage (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~basic_storage() = default;

	//-----------------------------------------------------------------------------
	//  Name : clear (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void clear() = 0;

	//-----------------------------------------------------------------------------
	//  Name : clear (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void clear(const std::string& group) = 0;
};

template <typename T>
struct asset_storage : public basic_storage
{
	/// aliases
	using request_container_t = std::unordered_map<std::string, core::task_future<asset_handle<T>>>;
	template <typename F>
	using callable = std::function<F>;
	using load_from_file_t = callable<bool(core::task_future<asset_handle<T>>&, const std::string&)>;
	using load_from_instance_t =
		callable<bool(core::task_future<asset_handle<T>>&, const std::string&, std::shared_ptr<T>)>;

	using predicate_t = callable<bool(const typename request_container_t::value_type&)>;
	//-----------------------------------------------------------------------------
	//  Name : ~storage ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	~asset_storage() override = default;

	void clear_with_condition(const predicate_t& predicate)
	{
		std::lock_guard<std::recursive_mutex> lock(container_mutex);
		for(auto it = container.cbegin(); it != container.cend();)
		{
			if(predicate(*it))
			{
				it = container.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	//-----------------------------------------------------------------------------
	//  Name : clear ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear() final
	{
		clear_with_condition([](const auto& it) {
			const auto& task = it.second;
			task.cancel();
			return true;
		});
	}

	//-----------------------------------------------------------------------------
	//  Name : clear ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear(const std::string& group) final
	{
		clear_with_condition([&group](const auto& it) {
			const auto& id = it.first;
			const auto& task = it.second;

			if(string_utils::begins_with(id, group))
			{
				task.cancel();
				return true;
			}
			return false;
		});
	}

	/// key, mode
	load_from_file_t load_from_file;

	/// key, mode
	load_from_instance_t load_from_instance;

	/// Storage container
	request_container_t container;

	/// Mutex
	std::recursive_mutex container_mutex;
};
}
