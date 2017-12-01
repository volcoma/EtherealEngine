#pragma once

#include <memory>
#include <string>

template <typename T>
struct asset_link
{
	std::string id;
	std::shared_ptr<T> asset;
};

template <typename T>
struct asset_handle
{
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
    
    inline std::shared_ptr<T> get_asset() const
    {
        return link->asset;
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
		if(!data)
		{
			link->id.clear();
		}
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
	asset_handle& operator=(std::shared_ptr<T> data)
	{
		// Own the specified handle's data pointer
		if(data != link->asset)
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
	inline bool operator==(const asset_handle& handle) const
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
	inline bool operator!=(const asset_handle& handle) const
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
	inline bool operator<(const asset_handle& handle) const
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
	inline bool operator>(const asset_handle& handle) const
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
	{ // test for non-null pointer
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
	T* operator->() const
	{ // return pointer to resource
		return get();
	}

	// Internal link to asset
	std::shared_ptr<asset_link<T>> link = std::make_shared<asset_link<T>>();
};
