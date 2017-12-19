#pragma once

#include <functional>
#include <unordered_map>

#include "core/common/nonstd/type_traits.hpp"
#include "core/string_utils/string_utils.h"
#include "core/system/task_system.h"

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
	using save_file_t = callable<void(const std::string&, const asset_handle<T>&)>;
	using rename_file_t = callable<void(const std::string&, const std::string&)>;
	using delete_file_t = callable<void(const std::string&)>;

    using predicate_t = callable<bool(const typename request_container_t::value_type&)>;
	//-----------------------------------------------------------------------------
	//  Name : ~storage ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	~asset_storage() = default;


    void clear_with_condition(const predicate_t& predicate)
    {
        std::lock_guard<std::recursive_mutex> lock(container_mutex);
        for (auto it = container.cbegin(); it != container.cend();)
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
	virtual void clear() final
	{
        clear_with_condition([](const auto& it)
        {
            const auto& task = it.second;
            task.wait();
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
	virtual void clear(const std::string& group) final
	{
        clear_with_condition([&group](const auto& it)
        {
            const auto& id = it.first;
            const auto& task = it.second;

            if(string_utils::begins_with(id, group, true))
            {
                task.wait();
                return true;
            }
            return false;
        });
	}

	/// key, mode
	load_from_file_t load_from_file;

	/// key, mode
	load_from_instance_t load_from_instance;

	/// key, asset
	save_file_t save_to_file;

	/// key, new_key
	rename_file_t rename_asset_file;

	/// key
	delete_file_t delete_asset_file;

	/// Storage container
    request_container_t container;

	/// Mutex
	std::recursive_mutex container_mutex;
};

}
