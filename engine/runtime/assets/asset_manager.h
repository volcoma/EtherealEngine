#pragma once

#include <functional>
#include <unordered_map>

#include "asset_flags.h"
#include "asset_storage.h"
#include <cassert>

namespace runtime
{

class asset_manager
{
public:
	asset_manager();
	~asset_manager();
	//-----------------------------------------------------------------------------
	//  Name : clear ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear();

	//-----------------------------------------------------------------------------
	//  Name : clear ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear(const std::string& group);
	//-----------------------------------------------------------------------------
	//  Name : add_storage ()
	/// <summary>
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	template <typename S, typename... Args>
	asset_storage<S>& add_storage(Args&&... args)
	{
		auto operation = storages_.emplace(rtti::type_id<asset_storage<S>>().hash_code(),
										   std::make_unique<asset_storage<S>>(std::forward<Args>(args)...));

		return static_cast<asset_storage<S>&>(*operation.first->second);
	}

	template <typename T>
	core::task_future<asset_handle<T>> load(const std::string& key, load_mode mode = load_mode::sync,
											load_flags flags = load_flags::standard)
	{
		auto& storage = get_storage<T>();
		return load_asset_from_file_impl<T>(key, mode, flags, storage.container_mutex, storage.container,
											storage.load_from_file);
	}

	//-----------------------------------------------------------------------------
	//  Name : create_asset_from_memory ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	template <typename T>
	core::task_future<asset_handle<T>>
	create_asset_from_memory(const std::string& key, const std::uint8_t* data, const std::uint32_t& size,
							 load_mode mode = load_mode::sync, load_flags flags = load_flags::standard)
	{
		auto& storage = get_storage<T>();
		return create_asset_from_memory_impl<T>(key, data, size, mode, flags, storage.container_mutex,
												storage.container, storage.load_from_memory);
	}

	template <typename T>
	core::task_future<asset_handle<T>> find_asset_entry(const std::string& key)
	{
		auto& storage = get_storage<T>();
		return find_asset_impl<T>(key, storage.container_mutex, storage.container);
	}

	template <typename T>
	core::task_future<asset_handle<T>> load_asset_from_instance(const std::string& key,
																std::shared_ptr<T> entry)
	{
		auto& storage = get_storage<T>();
		return load_asset_from_instance_impl(key, entry, storage.container_mutex, storage.container,
											 storage.load_from_instance);
	}

	template <typename T>
	void rename_asset(const std::string& key, const std::string& new_key)
	{
		auto& storage = get_storage<T>();

		std::lock_guard<std::recursive_mutex> lock(storage.container_mutex);
		auto it = storage.container.find(key);
		if(it != storage.container.end())
		{
			auto& future = it->second;
			auto asset = future.get();
			asset.link->id = new_key;
			storage.container[new_key] = future;
			storage.container.erase(it);
		}
	}

	template <typename T>
	void clear_asset(const std::string& key)
	{
		auto& storage = get_storage<T>();

		std::lock_guard<std::recursive_mutex> lock(storage.container_mutex);
		auto it = storage.container.find(key);
		if(it != storage.container.end())
		{
			auto& future = it->second;

			auto asset = future.get();
			asset.link->asset.reset();
			asset.link->id.clear();

			storage.container.erase(it);
		}
	}

	//-----------------------------------------------------------------------------
	//  Name : save ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	template <typename T>
	void save(const asset_handle<T>& asset)
	{
		auto& storage = get_storage<T>();
		if(storage.save_to_file)
		{
			storage.save_to_file(asset.id(), asset);
		}
	}

private:
	//-----------------------------------------------------------------------------
	//  Name : load_asset_from_file_impl ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	template <typename T, typename F>
	core::task_future<asset_handle<T>>
	load_asset_from_file_impl(const std::string& key, load_mode mode, load_flags flags,
							  std::recursive_mutex& container_mutex,
							  typename asset_storage<T>::request_container_t& container, F&& load_func)
	{
		std::unique_lock<std::recursive_mutex> lock(container_mutex);
		auto it = container.find(key);
		if(it != std::end(container))
		{
			auto& future = it->second;
			if(flags == load_flags::reload && future.is_ready())
			{
				if(load_func)
				{
					load_func(future, key);
				}
			}
			auto future_copy = future;

			lock.unlock();

			if(mode == load_mode::sync)
			{
				future_copy.wait();
			}

			return future_copy;
		}

		auto& future = container[key];
		// Dispatch the loading
		if(load_func)
		{
			load_func(future, key);
		}

		return future;
	}

	//-----------------------------------------------------------------------------
	//  Name : create_asset_from_memory_impl ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	template <typename T, typename F>
	core::task_future<asset_handle<T>>&
	create_asset_from_memory_impl(const std::string& key, const std::uint8_t* data, const std::uint32_t& size,
								  load_mode /*mode*/, load_flags /*flags*/,
								  std::recursive_mutex& container_mutex,
								  typename asset_storage<T>::request_container_t& container, F&& load_func)
	{

		std::lock_guard<std::recursive_mutex> lock(container_mutex);
		auto it = container.find(key);
		if(it != std::end(container))
		{
			// If there is already a loading request.
			auto& future = it->second;
			return future;
		}

		auto& future = container[key];
		// Dispatch the loading
		if(load_func)
		{
			load_func(future, key, data, size);
		}

		return future;
	}

	template <typename T, typename F>
	core::task_future<asset_handle<T>>&
	load_asset_from_instance_impl(const std::string& key, std::shared_ptr<T> entry,
								  std::recursive_mutex& container_mutex,
								  typename asset_storage<T>::request_container_t& container, F&& load_func)
	{
		std::lock_guard<std::recursive_mutex> lock(container_mutex);
		auto& future = container[key];
		// Dispatch the loading
		if(load_func)
		{
			load_func(future, key, entry);
		}

		return future;
	}

	template <typename T>
	core::task_future<asset_handle<T>>
	find_asset_impl(const std::string& key, std::recursive_mutex& container_mutex,
					typename asset_storage<T>::request_container_t& container)
	{
		std::lock_guard<std::recursive_mutex> lock(container_mutex);
		auto it = container.find(key);
		if(it != container.end())
		{
			return it->second;
		}

		return {};
	}

	//-----------------------------------------------------------------------------
	//  Name : get_storage ()
	/// <summary>
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	template <typename S>
	asset_storage<S>& get_storage()
	{
		auto it = storages_.find(rtti::type_id<asset_storage<S>>().hash_code());
		assert(it != storages_.end());
		return (static_cast<asset_storage<S>&>(*it->second.get()));
	}
	/// Different storages
	std::unordered_map<std::size_t, std::unique_ptr<basic_storage>> storages_;
};
}
