#pragma once

#include <functional>
#include <unordered_map>

#include "core/common/type_traits.hpp"
#include "core/common/string.h"
#include "core/events/delegate.hpp"
#include "../system/filesystem.h"
#include "load_request.hpp"


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
			if (string_utils::beginsWith(id, protocol, true))
			{
				pair.second.waitUntilReady();
				container.erase(id);
			}
		}
	}

	//-----------------------------------------------------------------------------
	//  Name : loadFromMemoryDefault ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void loadFromMemoryDefault(const fs::path&, const std::uint8_t*, std::uint32_t, LoadRequest<T>&) {}
	
	//-----------------------------------------------------------------------------
	//  Name : loadFromFileDefault ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void loadFromFileDefault(const fs::path&, const fs::path&, bool, LoadRequest<T>&) {}
	
	//-----------------------------------------------------------------------------
	//  Name : saveToFileDefault ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void saveToFileDefault(const fs::path&, const AssetHandle<T>&) {}

	/// key, data, size, outRequest
	delegate<void(const std::string&, const std::uint8_t*, std::uint32_t, LoadRequest<T>&)> loadFromMemory = loadFromMemoryDefault;

	/// key, absolutKey, async, outReqeust
	delegate<void(const std::string&, const fs::path&, bool, LoadRequest<T>&)> loadFromFile = loadFromFileDefault;

	/// absolutKey, asset
	delegate<void(const fs::path&, const AssetHandle<T>&)> saveToFile = saveToFileDefault;

	/// Storage container
	std::unordered_map<std::string, LoadRequest<T>> container;
	/// Sub directory
	fs::path subdir;
	/// Sub directory
	fs::path platform;
};

template<typename T>
inline fs::path getAbsoluteKey(const std::string& toLowerKey, T storage)
{
	fs::path absoluteKey = fs::resolve_protocol(toLowerKey);
	fs::path dir = absoluteKey;
	dir.remove_filename();
	std::string file = absoluteKey.filename().string();

	static const std::string ext = ".asset";
	absoluteKey = fs::absolute(dir / storage->subdir / storage->platform / fs::path(file + ext));
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
	//  Name : getStorage ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	template <typename S>
	std::shared_ptr<TStorage<S>> getStorage()
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
	//  Name : createAssetFromMemory ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	template<typename T>
	LoadRequest<T>& createAssetFromMemory(
		const std::string& key,
		const std::uint8_t* data,
		const std::uint32_t& size)
	{
		auto storage = getStorage<T>();
		const std::string toLowerKey = string_utils::toLower(key);
		return createAssetFromMemoryImpl<T>(toLowerKey, data, size, storage->container, storage->loadFromMemory);
	}

	//-----------------------------------------------------------------------------
	//  Name : renameAsset ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	template<typename T>
	void renameAsset(
		const std::string& key,
		const std::string& newRelativeKey)
	{
		auto storage = getStorage<T>();
		const std::string toLowerKey = string_utils::toLower(key);
		const std::string toLowerNewKey = string_utils::toLower(newRelativeKey);
		
		auto absoluteKey = getAbsoluteKey(toLowerKey, storage);
		auto absoluteNewKey = getAbsoluteKey(toLowerNewKey, storage);

		fs::rename(absoluteKey, absoluteNewKey, std::error_code{});

		storage->container[toLowerNewKey] = storage->container[toLowerKey];
		storage->container[toLowerNewKey].asset.link->id = toLowerNewKey;
		storage->container.erase(toLowerKey);
	}

	//-----------------------------------------------------------------------------
	//  Name : clearAsset ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	template<typename T>
	void clearAsset(
		const std::string& key)
	{
		auto storage = getStorage<T>();
		const std::string toLowerKey = string_utils::toLower(key);

		auto& request = storage->container[toLowerKey];
		request.asset.link->asset.reset();
		request.asset.link->id.clear();
		storage->container.erase(toLowerKey);
	}

	//-----------------------------------------------------------------------------
	//  Name : clearAsset ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	template<typename T>
	void deleteAsset(
		const std::string& key)
	{
		auto storage = getStorage<T>();
		const std::string toLowerKey = string_utils::toLower(key);
		const fs::path absoluteKey = getAbsoluteKey(toLowerKey, storage);
		fs::remove(absoluteKey, std::error_code{});

		auto& request = storage->container[toLowerKey];
		request.asset.link->asset.reset();
		request.asset.link->id.clear();
		storage->container.erase(toLowerKey);
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
		const auto toLowerKey = string_utils::toLower(key);
		auto storage = getStorage<T>();
		//if embedded resource
		if (toLowerKey.find("embedded") != std::string::npos)
		{
			return findOrCreateAssetImpl<T>(toLowerKey, storage->container);
		}
		else
		{
			const fs::path absoluteKey = getAbsoluteKey(toLowerKey, storage);
			return loadAssetFromFileImpl<T>(toLowerKey, absoluteKey, async, force, storage->container, storage->loadFromFile);

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
		auto storage = getStorage<T>();
		const std::string toLowerKey = string_utils::toLower(asset.id());
		const fs::path absoluteKey = getAbsoluteKey(toLowerKey, storage);
		storage->saveToFile(absoluteKey, asset);
	}

private:
	//-----------------------------------------------------------------------------
	//  Name : loadAssetFromFileImpl ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	template<typename T, typename F>
	LoadRequest<T>& loadAssetFromFileImpl(
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
			else if (!async && !request.isReady())
			{
				request.waitUntilReady();
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
			auto& request = findOrCreateAssetImpl(key, container);
			//Dispatch the loading
			loadFunc(key, absoluteKey, async, request);

			return request;
		}
	}

	//-----------------------------------------------------------------------------
	//  Name : createAssetFromMemoryImpl ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	template<typename T, typename F>
	LoadRequest<T>& createAssetFromMemoryImpl(
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
			auto& request = findOrCreateAssetImpl(key, container);
			//Dispatch the loading
			loadFunc(key, data, size, request);

			return request;
		}

	}

	//-----------------------------------------------------------------------------
	//  Name : findOrCreateAssetImpl ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	template<typename T>
	LoadRequest<T>& findOrCreateAssetImpl(
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
