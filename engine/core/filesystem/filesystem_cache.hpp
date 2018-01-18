#pragma once

#include "filesystem_watcher.h"
#include <chrono>
#include <type_traits>
#include <vector>

namespace fs
{

template <typename T>
class cache
{
public:
	using iterator_t = T;
	static_assert(std::is_same<iterator_t, recursive_directory_iterator>::value ||
					  std::is_same<iterator_t, directory_iterator>::value,
				  "T must be a valid directory iterator type");

	using clock_t = std::chrono::high_resolution_clock;
	cache(const fs::path& p, clock_t::duration scan_frequency)
		: _path(p)
		, _scan_frequency(scan_frequency)
	{
		watch();
	}

	cache(const cache& rhs)
		: _path(rhs.path)
		, _scan_frequency(rhs._scan_frequency)
		, _entries(rhs._entries)
		, _should_refresh(rhs._should_refresh)
	{
		watch();
	}

	cache(cache&& rhs)
		: _path(std::move(rhs._path))
		, _scan_frequency(std::move(rhs._scan_frequency))
		, _entries(std::move(rhs._entries))
		, _should_refresh(std::move(rhs._should_refresh))
	{
		watch();
	}

	cache& operator=(const cache& rhs)
	{
		unwatch();
		_path = rhs._path;
		_scan_frequency = rhs._scan_frequency;
		_entries = rhs._entries;
		_should_refresh = rhs._should_refresh;
		watch();

		return *this;
	}

	cache& operator=(cache&& rhs)
	{
		unwatch();
		_path = std::move(rhs._path);
		_scan_frequency = std::move(rhs._scan_frequency);
		_entries = std::move(rhs._entries);
		_should_refresh = std::move(rhs._should_refresh);
		watch();

		return *this;
	}

	~cache()
	{
		unwatch();
	}
	//-----------------------------------------------------------------------------
	//  Name : end ()
	/// <summary>
	/// Returns the begin iterator for the underlying cached container and also
	/// refreshes the container if needed.
	/// </summary>
	//-----------------------------------------------------------------------------
	decltype(auto) begin() const
	{
		if(should_refresh())
		{
			refresh();
		}
		return _entries.begin();
	}

	//-----------------------------------------------------------------------------
	//  Name : end ()
	/// <summary>
	/// Returns the end iterator for the underlying cached container.
	/// </summary>
	//-----------------------------------------------------------------------------
	decltype(auto) end() const
	{
		return _entries.end();
	}

	//-----------------------------------------------------------------------------
	//  Name : refresh ()
	/// <summary>
	/// Refreshes the cache and updates the timestamp of the last refresh.
	/// This operation is slow so try to not call it often. By default it is called
	/// only if iterated and marked by the dir watcher
	/// since its last refresh.
	/// </summary>
	//-----------------------------------------------------------------------------
	void refresh() const
	{
		_entries.clear();

		fs::error_code err;
		for(const auto& p : iterator_t(_path, err))
		{
			_entries.push_back(p);
		}

		_should_refresh = false;
	}

private:
	//-----------------------------------------------------------------------------
	//  Name : should_refresh ()
	/// <summary>
	/// Checks whether the internal container should be refreshed.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool should_refresh() const
	{
		return _should_refresh;
	}

	void watch()
	{
		using namespace std::literals;
		constexpr bool is_recursive = std::is_same<iterator_t, recursive_directory_iterator>::value;

		_watch_id = watcher::watch(_path / "*", is_recursive, false, _scan_frequency,
								   [this](const auto&, bool) { _should_refresh = true; });
	}
	void unwatch()
	{
		watcher::unwatch(_watch_id);
	}

	///
	fs::path _path;

	clock_t::duration _scan_frequency = std::chrono::milliseconds(500);
	///
	mutable std::vector<directory_entry> _entries;
	///
	mutable std::atomic_bool _should_refresh = {true};
	///
	std::uint64_t _watch_id = 0;
};

using directory_cache = cache<directory_iterator>;
using recursive_directory_cache = cache<recursive_directory_iterator>;

//////////////////////////////////////////////////////////////////////
/// Usage:
///
/// using namespace std::literals;
///
/// fs::recursive_directory_cache cache("/home/test", 500ms);
/// //fs::directory_cache cache("/home/test", 500ms);
///
/// while(!should_exit)
/// {
///     // cache will be updated only if iterated and 500 ms have passed
///     // since its last update
/// 	for(const auto& p : cache)
/// 	{
/// 	}
/// }
}
