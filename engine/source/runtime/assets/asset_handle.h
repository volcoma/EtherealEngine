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
	//-----------------------------------------------------------------------------
	//  Name : AssetHandle ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	AssetHandle() = default;

	//-----------------------------------------------------------------------------
	//  Name : AssetHandle ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	AssetHandle(const std::string& id)
	{
		link->id = id;
	}

	//-----------------------------------------------------------------------------
	//  Name : AssetHandle ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	AssetHandle(const std::string& id, std::shared_ptr<T> data)
	{
		link->id = id;
		link->asset = data;
	}

	//-----------------------------------------------------------------------------
	//  Name : get ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline T* get() const
	{
		return link->asset.get();
	}

	//-----------------------------------------------------------------------------
	//  Name : reset ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void reset(std::shared_ptr<T> data = nullptr)
	{
		link->asset = data;
		if (!data)
			link->id.clear();
	}

	//-----------------------------------------------------------------------------
	//  Name : use_count ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline long use_count() const
	{
		return link.use_count();
	}

	//-----------------------------------------------------------------------------
	//  Name : id ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::string& id() const
	{
		return link->id;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator= ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	AssetHandle & operator=(const AssetHandle & handle)
	{
		// Share the specified handle's data pointer
		if (link != handle.link)
			link = handle.link;

		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator= ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	AssetHandle & operator=(std::shared_ptr<T> data)
	{
		// Own the specified handle's data pointer
		if (data != link->asset)
			link->asset = data;

		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator== ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool operator==(const AssetHandle & handle) const
	{
		return (get() == handle.get());
	}

	//-----------------------------------------------------------------------------
	//  Name : operator!= ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool operator!=(const AssetHandle & handle) const
	{
		return (get() != handle.get());
	}

	//-----------------------------------------------------------------------------
	//  Name : operator< ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool operator<(const AssetHandle & handle) const
	{
		return (get() < handle.get());
	}

	//-----------------------------------------------------------------------------
	//  Name : operator> ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool operator>(const AssetHandle & handle) const
	{
		return (get() > handle.get());
	}

	//-----------------------------------------------------------------------------
	//  Name : operator bool ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	explicit operator bool() const
	{	// test for non-null pointer
		return (get() != nullptr);
	}

	//-----------------------------------------------------------------------------
	//  Name : operator-> ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	T *operator->() const
	{	// return pointer to resource
		return get();
	}

	// Internal link to asset
	std::shared_ptr<AssetLink<T>> link = std::make_shared<AssetLink<T>>();
};
