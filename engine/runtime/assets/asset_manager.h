#pragma once

#include <functional>
#include <unordered_map>

#include "core/nonstd/type_traits.hpp"
#include "core/common/string.h"
#include "core/signals/event.hpp"
#include "../system/filesystem.h"
#include "load_request.hpp"

namespace runtime
{
	/// aliases
	template<typename T>
	using request_container_t = std::unordered_map<std::string, load_request<T>>;

	struct base_storage
	{
		//-----------------------------------------------------------------------------
		//  Name : ~base_storage (virtual )
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		virtual ~base_storage() = default;

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
		virtual void clear(const std::string& protocol) = 0;

	};

	template<typename T>
	struct asset_storage : public base_storage
	{
		//-----------------------------------------------------------------------------
		//  Name : ~storage ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		~asset_storage() = default;

		//-----------------------------------------------------------------------------
		//  Name : clear ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void clear()
		{
			container.clear();
		}

		//-----------------------------------------------------------------------------
		//  Name : clear ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void clear(const std::string& protocol)
		{
			auto containerCopy = container;
			for (auto& pair : containerCopy)
			{
				const auto& id = pair.first;
				if (string_utils::begins_with(id, protocol, true))
				{
					pair.second.wait_until_ready();
					container.erase(id);
				}
			}
		}

		/// key, data, size, outRequest
		delegate<void(const std::string&, const std::uint8_t*, std::uint32_t, load_request<T>&)> load_from_memory = [](const fs::path&, const std::uint8_t*, std::uint32_t, load_request<T>&) {};

		/// key, absolutKey, async, outReqeust
		delegate<void(const std::string&, const fs::path&, bool, load_request<T>&)> load_from_file = [](const fs::path&, const fs::path&, bool, load_request<T>&) {};

		/// absolutKey, asset
		delegate<void(const fs::path&, const asset_handle<T>&)> save_to_file = [](const fs::path&, const asset_handle<T>&) {};

		/// Storage container
		std::unordered_map<std::string, load_request<T>> container;
		/// Extension
		std::string ext;
	};

	template<typename T>
	inline fs::path get_absolute_key(const std::string& to_lower_key, T storage)
	{
		fs::path absolute_key = fs::absolute(fs::resolve_protocol(to_lower_key).string() + storage->ext);
		return absolute_key;
	};


	class asset_manager : public core::subsystem
	{
	public:
		bool initialize();
		//-----------------------------------------------------------------------------
		//  Name : add ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template <typename S>
		void add(std::shared_ptr<asset_storage<S>> system)
		{
			storages.insert(std::make_pair(rtti::type_id<asset_storage<S>>().hash_code(), system));
		}

		//-----------------------------------------------------------------------------
		//  Name : add ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template <typename S, typename ... Args>
		std::shared_ptr<asset_storage<S>> add(Args && ... args)
		{
			auto s = std::make_shared<asset_storage<S>>(std::forward<Args>(args) ...);
			add(s);
			return s;
		}

		//-----------------------------------------------------------------------------
		//  Name : get_storage ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template <typename S>
		std::shared_ptr<asset_storage<S>> get_storage()
		{
			auto it = storages.find(rtti::type_id<asset_storage<S>>().hash_code());
			//assert(it != storages.end());
			return it == storages.end()
				? std::shared_ptr<asset_storage<S>>()
				: std::shared_ptr<asset_storage<S>>(std::static_pointer_cast<asset_storage<S>>(it->second));
		}

		//-----------------------------------------------------------------------------
		//  Name : clear ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void clear()
		{
			for (auto& pair : storages)
			{
				auto& storage = pair.second;
				storage->clear();
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
		void clear(const std::string& protocol)
		{
			for (auto& pair : storages)
			{
				auto& storage = pair.second;
				storage->clear(protocol);
			}
		}

		//-----------------------------------------------------------------------------
		//  Name : create_asset_from_memory ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename T>
		load_request<T>& create_asset_from_memory(
			const std::string& key,
			const std::uint8_t* data,
			const std::uint32_t& size)
		{
			auto storage = get_storage<T>();
			return create_asset_from_memory_impl<T>(key, data, size, storage->container, storage->load_from_memory);
		}

		//-----------------------------------------------------------------------------
		//  Name : rename_asset ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename T>
		void rename_asset(
			const std::string& key,
			const std::string& new_key)
		{
			auto storage = get_storage<T>();

			auto absolute_key = get_absolute_key(key, storage);
			auto absolute_new_key = get_absolute_key(new_key, storage);
			
			// rename compiled assets
			fs::rename(absolute_key, absolute_new_key, std::error_code{});

			auto& request = storage->container[key];
			storage->container[new_key] = request;
			storage->container[new_key].asset.link->id = new_key;
			storage->container.erase(key);
		}

		//-----------------------------------------------------------------------------
		//  Name : clear_asset ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename T>
		void clear_asset(
			const std::string& key)
		{
			auto storage = get_storage<T>();
		
			auto& request = storage->container[key];
			request.asset.link->asset.reset();
			request.asset.link->id.clear();
			storage->container.erase(key);
		}

		//-----------------------------------------------------------------------------
		//  Name : delete_asset ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename T>
		void delete_asset(
			const std::string& key)
		{
			auto storage = get_storage<T>();
			fs::path absolute_key = get_absolute_key(key, storage);

			fs::remove(absolute_key, std::error_code{});

			auto& request = storage->container[key];
			request.asset.link->asset.reset();
			request.asset.link->id.clear();
			storage->container.erase(key);
		}
		//-----------------------------------------------------------------------------
		//  Name : load ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename T>
		load_request<T>& load(
			const std::string& key,
			bool async,
			bool force = false)
		{
			auto storage = get_storage<T>();
			//if embedded resource
			if (key.find("embedded") != std::string::npos)
			{
				return find_or_create_asset_impl<T>(key, storage->container);
			}
			else
			{
				const fs::path absoluteKey = get_absolute_key(key, storage);
				return load_asset_from_file_impl<T>(key, absoluteKey, async, force, storage->container, storage->load_from_file);

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
		template<typename T>
		void save(const asset_handle<T>& asset)
		{
			auto storage = get_storage<T>();
			const fs::path absoluteKey = get_absolute_key(asset.id(), storage);
			storage->save_to_file(absoluteKey, asset);
		}

		template<typename T>
		load_request<T>& find_or_create_asset_entry(
			const std::string& key
		)
		{
			auto storage = get_storage<T>();
			return find_or_create_asset_impl<T>(key, storage->container);
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
		template<typename T, typename F>
		load_request<T>& load_asset_from_file_impl(
			const std::string& key,
			const fs::path& absoluteKey,
			bool async,
			bool force,
			request_container_t<T>& container,
			F&& loadFunc
		)
		{

			auto it = container.find(key);
			if (it != std::end(container))
			{
				auto& request = it->second;

				if (force)
				{
					loadFunc(key, absoluteKey, async, request);
				}
				else if (!async && !request.is_ready())
				{
					request.wait_until_ready();
				}

				return request;
			}
			else if (!fs::exists(absoluteKey, std::error_code{}))
			{
				static load_request<T> emptyRequest;
				return emptyRequest;
			}
			else
			{
				auto& request = find_or_create_asset_impl(key, container);
				//Dispatch the loading
				loadFunc(key, absoluteKey, async, request);

				return request;
			}
		}

		//-----------------------------------------------------------------------------
		//  Name : create_asset_from_memory_impl ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename T, typename F>
		load_request<T>& create_asset_from_memory_impl(
			const std::string& key,
			const std::uint8_t* data,
			const std::uint32_t& size,
			request_container_t<T>& container,
			F&& loadFunc
		)
		{

			auto it = container.find(key);
			if (it != std::end(container))
			{
				// If there is already a loading request.
				auto& request = it->second;
				return request;
			}
			else
			{
				auto& request = find_or_create_asset_impl(key, container);
				//Dispatch the loading
				loadFunc(key, data, size, request);

				return request;
			}

		}

		//-----------------------------------------------------------------------------
		//  Name : find_or_create_asset_impl ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename T>
		load_request<T>& find_or_create_asset_impl(
			const std::string& key,
			request_container_t<T>& container
		)
		{
			auto& request = container[key];
			return request;
		}

		/// Different storages
		std::unordered_map<std::size_t, std::shared_ptr<base_storage>> storages;
	};

}