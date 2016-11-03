#pragma once

#include <string>
#include <memory>


template<typename T>
struct AssetLink
{
	std::string id;
	std::shared_ptr<T> asset;
};

template<typename T>
struct AssetHandle
{
	AssetHandle()
	{}

	AssetHandle(const std::string& id)
	{
		link->id = id;
	}

	AssetHandle(const std::string& id, std::shared_ptr<T> data)
	{
		link->id = id;
		link->asset = data;
	}
	inline T* get() const
	{
		return link->asset.get();
	}

	inline void reset(std::shared_ptr<T> data = nullptr)
	{
		link->asset = data;
	}

	inline long use_count() const
	{
		return link.use_count();
	}

	const std::string& id() const
	{
		return link->id;
	}

	AssetHandle & operator=(const AssetHandle & handle)
	{
		// Share the specified handle's data pointer
		if(link != handle.link)
			link = handle.link;

		return *this;
	}

	AssetHandle & operator=(std::shared_ptr<T> data)
	{
		// Own the specified handle's data pointer
		if (data != link->asset)
			link->asset = data;

		return *this;
	}

	inline bool operator==(const AssetHandle & handle) const
	{
		return (get() == handle.get());
	}
	inline bool operator!=(const AssetHandle & handle) const
	{
		return (get() != handle.get());
	}
	inline bool operator<(const AssetHandle & handle) const
	{
		return (get() < handle.get());
	}
	inline bool operator>(const AssetHandle & handle) const
	{
		return (get() > handle.get());
	}

	explicit operator bool() const
	{	// test for non-null pointer
		return (get() != nullptr);
	}

	T *operator->() const
	{	// return pointer to resource
		return get();
	}
	std::shared_ptr<AssetLink<T>> link = std::make_shared<AssetLink<T>>();
};
