#pragma once

#include <functional>
#include <unordered_map>

#include "Core/common/string_utils.h"
#include "Core/events/delegate.hpp"
#include "../System/FileSystem.h"
#include "LoadRequest.hpp"

/// aliases
template<typename T>
using RequestContainer = std::unordered_map<std::string, LoadRequest<T>>;

struct Storage
{
	typedef std::size_t Family;
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
	//  Name : getCounter ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static Family& getCounter()
	{
		static Family family_counter_;
		return family_counter_;
	}
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
	//  Name : family ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static Family family()
	{
		static Family family = getCounter()++;
		return family;
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
		container.clear();
	}

	//-----------------------------------------------------------------------------
	//  Name : loadFromMemoryDefault ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void loadFromMemoryDefault(const std::string&, const std::uint8_t*, std::uint32_t, LoadRequest<T>&) {}
	
	//-----------------------------------------------------------------------------
	//  Name : loadFromFileDefault ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void loadFromFileDefault(const std::string&, const std::string&, bool, LoadRequest<T>&) {}
	
	//-----------------------------------------------------------------------------
	//  Name : saveToFileDefault ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void saveToFileDefault(const std::string&, const AssetHandle<T>&) {}

	/// relativeKey, data, size, outRequest
	delegate<void(const std::string&, const std::uint8_t*, std::uint32_t, LoadRequest<T>&)> loadFromMemory = loadFromMemoryDefault;

	/// relativeKey, absolutKey, async, outReqeust
	delegate<void(const std::string&, const std::string&, bool, LoadRequest<T>&)> loadFromFile = loadFromFileDefault;

	/// absolutKey, asset
	delegate<void(const std::string&, const AssetHandle<T>&)> saveToFile = saveToFileDefault;

	/// Storage container
	std::unordered_map<std::string, LoadRequest<T>> container;
	/// Sub directory
	std::string subdir;
};

class AssetManager
{
public:

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
		storages.insert(std::make_pair(TStorage<S>::family(), system));
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
		auto it = storages.find(TStorage<S>::family());
		assert(it != storages.end());
		return it == storages.end()
			? std::shared_ptr<TStorage<S>>()
			: std::shared_ptr<TStorage<S>>(std::static_pointer_cast<TStorage<S>>(it->second));
	}
	//-----------------------------------------------------------------------------
	//  Name : ~AssetManager ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~AssetManager()
	{
		shutdown();
	}

	//-----------------------------------------------------------------------------
	//  Name : shutdown ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void shutdown()
	{

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
	//  Name : createAssetFromMemory ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	template<typename T>
	LoadRequest<T>& createAssetFromMemory(
		const std::string& relativeKey,
		const std::uint8_t* data,
		const std::uint32_t& size)
	{
		auto storage = getStorage<T>();
		const std::string toLowerKey = string_utils::toLower(relativeKey);
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
		const std::string& relativeKey,
		const std::string& newRelativeKey)
	{
		auto storage = getStorage<T>();
		const std::string toLowerKey = string_utils::toLower(relativeKey);
		const std::string toLowerNewKey = string_utils::toLower(newRelativeKey);
		auto getAbsoluteKey = [](const std::string& toLowerKey, auto storage)
		{

			std::string absoluteKey = fs::resolveFileLocation(toLowerKey);
			std::string dir = fs::getDirectoryName(absoluteKey);
			std::string file = fs::getFileName(absoluteKey);
			dir = string_utils::replace(dir, '\\', '/');
			dir = string_utils::toLower(dir);
			file = string_utils::toLower(file);
			static const std::string ext = ".asset";
			absoluteKey = dir + storage->subdir + file + ext;
			return absoluteKey;
		};

		auto absoluteKey = getAbsoluteKey(toLowerKey, storage);
		auto absoluteNewKey = getAbsoluteKey(toLowerNewKey, storage);
		fs::moveFile(absoluteKey, absoluteNewKey, true);

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
		const std::string& relativeKey)
	{
		auto storage = getStorage<T>();
		const std::string toLowerKey = string_utils::toLower(relativeKey);

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
		const std::string& relativeKey,
		bool async,
		bool force = false)
	{
		const std::string toLowerKey = string_utils::toLower(relativeKey);
		auto storage = getStorage<T>();
		//if embedded resource
		if (toLowerKey.find("embedded") != std::string::npos)
		{
			return findOrCreateAssetImpl<T>(toLowerKey, storage->container);
		}
		else
		{
			std::string absoluteKey = fs::resolveFileLocation(toLowerKey);
			std::string dir = fs::getDirectoryName(absoluteKey);
			std::string file = fs::getFileName(absoluteKey);
			dir = string_utils::replace(dir, '\\', '/');
			dir = string_utils::toLower(dir);
			file = string_utils::toLower(file);
			static const std::string ext = ".asset";
			absoluteKey = dir + storage->subdir + file + ext;

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
		std::string absoluteKey = fs::resolveFileLocation(toLowerKey);
		std::string dir = fs::getDirectoryName(absoluteKey);
		std::string file = fs::getFileName(absoluteKey);
		dir = string_utils::replace(dir, '\\', '/');
		dir = string_utils::toLower(dir);
		file = string_utils::toLower(file);
		static const std::string ext = ".asset";
		absoluteKey = dir + storage->subdir + file + ext;

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
		const std::string& relativeKey,
		const std::string& absoluteKey,
		bool async,
		bool force,
		RequestContainer<T>& container,
		F&& loadFunc
	)
	{

		auto it = container.find(relativeKey);
		if (it != std::end(container))
		{
			auto& request = it->second;

			if (force)
			{
				loadFunc(relativeKey, absoluteKey, async, request);
			}
			else if (!async && !request.isReady())
			{
				request.waitUntilReady();
			}

			return request;
		}
		else if (!fs::fileExists(absoluteKey))
		{
			static LoadRequest<T> emptyRequest;
			return emptyRequest;
		}
		else
		{
			auto& request = findOrCreateAssetImpl(relativeKey, container);
			//Dispatch the loading
			loadFunc(relativeKey, absoluteKey, async, request);

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
		const std::string& relativeKey,
		const std::uint8_t* data,
		const std::uint32_t& size,
		RequestContainer<T>& container,
		F&& loadFunc
	)
	{

		auto it = container.find(relativeKey);
		if (it != std::end(container))
		{
			// If there is already a loading request.
			auto& request = it->second;
			return request;
		}
		else
		{
			auto& request = findOrCreateAssetImpl(relativeKey, container);
			//Dispatch the loading
			loadFunc(relativeKey, data, size, request);

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
		const std::string& relativeKey,
		RequestContainer<T>& container
	)
	{
		auto& request = container[relativeKey];
		return request;
	}

	/// Different storages
	std::unordered_map<Storage::Family, std::shared_ptr<Storage>> storages;
};
