#pragma once

#include <functional>
#include <unordered_map>

#include "core/common/type_traits.hpp"
#include "core/common/string.h"
#include "core/events/delegate.hpp"
#include "../system/filesystem.h"
#include "load_request.hpp"

namespace runtime
{
	/// aliases
	template<typename T>
	using RequestContainer = std::unordered_map<std::string, LoadRequest<T>>;

	struct Storage
	{
		//-----------------------------------------------------------------------------
		//  Name : ~Storage (virtual )
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		virtual ~Storage() = default;

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
	struct TStorage : Storage
	{
		//-----------------------------------------------------------------------------
		//  Name : ~TStorage ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		~TStorage() = default;

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

		//-----------------------------------------------------------------------------
		//  Name : load_from_memory_default ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		static void load_from_memory_default(const fs::path&, const std::uint8_t*, std::uint32_t, LoadRequest<T>&) {}

		//-----------------------------------------------------------------------------
		//  Name : load_from_file_default ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		static void load_from_file_default(const fs::path&, const fs::path&, bool, LoadRequest<T>&) {}

		//-----------------------------------------------------------------------------
		//  Name : save_to_file_default ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		static void save_to_file_default(const fs::path&, const AssetHandle<T>&) {}

		/// key, data, size, outRequest
		delegate<void(const std::string&, const std::uint8_t*, std::uint32_t, LoadRequest<T>&)> load_from_memory = load_from_memory_default;

		/// key, absolutKey, async, outReqeust
		delegate<void(const std::string&, const fs::path&, bool, LoadRequest<T>&)> load_from_file = load_from_file_default;

		/// absolutKey, asset
		delegate<void(const fs::path&, const AssetHandle<T>&)> save_to_file = save_to_file_default;

		/// Storage container
		std::unordered_map<std::string, LoadRequest<T>> container;
		/// Extention
		std::string ext;
	};

	template<typename T>
	inline fs::path get_absolute_key(const std::string& toLowerKey, T storage)
	{
		fs::path absoluteKey = fs::resolve_protocol(toLowerKey);
		fs::path dir = absoluteKey;
		dir.remove_filename();
		std::string file = absoluteKey.filename().string();

		absoluteKey = fs::absolute(dir / fs::path(file + storage->ext));
		return absoluteKey;
	};


	class AssetManager : public core::Subsystem
	{
	public:
		void setup();
		//-----------------------------------------------------------------------------
		//  Name : add ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template <typename S>
		void add(std::shared_ptr<TStorage<S>> system)
		{
			storages.insert(std::make_pair(core::TypeInfo::id<Storage, TStorage<S>>(), system));
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
		std::shared_ptr<TStorage<S>> add(Args && ... args)
		{
			auto s = std::make_shared<TStorage<S>>(std::forward<Args>(args) ...);
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
		std::shared_ptr<TStorage<S>> get_storage()
		{
			auto it = storages.find(core::TypeInfo::id<Storage, TStorage<S>>());
			assert(it != storages.end());
			return it == storages.end()
				? std::shared_ptr<TStorage<S>>()
				: std::shared_ptr<TStorage<S>>(std::static_pointer_cast<TStorage<S>>(it->second));
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
		LoadRequest<T>& create_asset_from_memory(
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
			const std::string& newRelativeKey)
		{
			auto storage = get_storage<T>();

			auto absoluteKey = get_absolute_key(key, storage);
			auto absoluteNewKey = get_absolute_key(newRelativeKey, storage);

			fs::rename(absoluteKey, absoluteNewKey, std::error_code{});

			storage->container[newRelativeKey] = storage->container[key];
			storage->container[newRelativeKey].asset.link->id = newRelativeKey;
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
			const fs::path absoluteKey = get_absolute_key(key, storage);
			fs::remove(absoluteKey, std::error_code{});

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
		LoadRequest<T>& load(
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
		void save(const AssetHandle<T>& asset)
		{
			auto storage = get_storage<T>();
			const fs::path absoluteKey = get_absolute_key(asset.id(), storage);
			storage->save_to_file(absoluteKey, asset);
		}

		template<typename T>
		void create_asset_entry(
			const std::string& key
		)
		{
			auto storage = get_storage<T>();
			find_or_create_asset_impl<T>(key, storage->container);
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
		LoadRequest<T>& load_asset_from_file_impl(
			const std::string& key,
			const fs::path& absoluteKey,
			bool async,
			bool force,
			RequestContainer<T>& container,
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
				static LoadRequest<T> emptyRequest;
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
		LoadRequest<T>& create_asset_from_memory_impl(
			const std::string& key,
			const std::uint8_t* data,
			const std::uint32_t& size,
			RequestContainer<T>& container,
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
		LoadRequest<T>& find_or_create_asset_impl(
			const std::string& key,
			RequestContainer<T>& container
		)
		{
			auto& request = container[key];
			return request;
		}

		/// Different storages
		std::unordered_map<core::TypeInfo::index_t, std::shared_ptr<Storage>> storages;
	};

}