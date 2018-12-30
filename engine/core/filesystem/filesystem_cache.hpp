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
	cache() = default;

	cache(const fs::path& p, clock_t::duration scan_frequency)
		: path_(p)
		, scan_frequency_(scan_frequency)
		, should_refresh_(true)
	{
		watch();
	}

	cache(const cache& rhs)
		: path_(rhs.path)
		, scan_frequency_(rhs.scan_frequency_)
		, entries_(rhs.entries_)
		, should_refresh_(rhs.should_refresh_.load())
	{
		watch();
	}

	cache(cache&& rhs) noexcept
		: path_(std::move(rhs.path_))
		, scan_frequency_(std::move(rhs.scan_frequency_))
		, entries_(std::move(rhs.entries_))
		, should_refresh_(rhs.should_refresh_.load())
	{
		watch();
	}

	cache& operator=(const cache& rhs)
	{
		unwatch();
		path_ = rhs.path_;
		scan_frequency_ = rhs.scan_frequency_;
		entries_ = rhs.entries_;
		should_refresh_ = rhs.should_refresh_.load();
		watch();

		return *this;
	}

	cache& operator=(cache&& rhs) noexcept
	{
		unwatch();
		path_ = std::move(rhs.path_);
		scan_frequency_ = std::move(rhs.scan_frequency_);
		entries_ = std::move(rhs.entries_);
		should_refresh_ = rhs.should_refresh_.load();
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
		return entries_.begin();
	}

	//-----------------------------------------------------------------------------
	//  Name : end ()
	/// <summary>
	/// Returns the end iterator for the underlying cached container.
	/// </summary>
	//-----------------------------------------------------------------------------
	decltype(auto) end() const
	{
		return entries_.end();
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
		entries_.clear();

		fs::error_code err;
		iterator_t it(path_, err);
		for(const auto& p : it)
		{
			entries_.emplace_back();
			auto& cache_entry = entries_.back();
			cache_entry.entry = p;
			const auto& absolute_path = cache_entry.entry.path();
			cache_entry.relative = fs::convert_to_protocol(absolute_path).generic_string();
			cache_entry.extension = absolute_path.extension().string();

			auto filename = absolute_path.filename();
			cache_entry.name = filename.string();

			while(filename.has_extension())
			{
				filename = filename.stem();
				cache_entry.name = filename.string();
			}
		}

		std::sort(std::begin(entries_), std::end(entries_), [](const auto& lhs, const auto& rhs) {
			return fs::is_directory(lhs.entry.status()) > fs::is_directory(rhs.entry.status());
		});

		should_refresh_ = false;
	}

	const fs::path& get_path() const
	{
		return path_;
	}

	void set_path(const fs::path& path)
	{
		if(path_ == path)
		{
			return;
		}
		unwatch();
		path_ = path;
		should_refresh_ = true;
		watch();
	}

	void set_scan_frequency(clock_t::duration scan_frequency)
	{
		if(scan_frequency_ == scan_frequency)
		{
			return;
		}
		unwatch();
		scan_frequency_ = scan_frequency;
		should_refresh_ = true;
		watch();
	}

	struct entry_info
	{
        directory_entry entry;
        std::string relative;
        std::string name;
        std::string extension;
	};

private:
	//-----------------------------------------------------------------------------
	//  Name : should_refresh ()
	/// <summary>
	/// Checks whether the internal container should be refreshed.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool should_refresh() const
	{
		return should_refresh_;
	}

	void watch()
	{
		using namespace std::literals;
		constexpr bool is_recursive = std::is_same<iterator_t, recursive_directory_iterator>::value;

		watch_id_ = watcher::watch(path_ / "*", is_recursive, false, scan_frequency_,
								   [this](const auto&, bool) { should_refresh_ = true; });
	}
	void unwatch()
	{
		watcher::unwatch(watch_id_);
	}

	///
	fs::path path_;

	clock_t::duration scan_frequency_ = std::chrono::milliseconds(500);
	///
	mutable std::vector<entry_info> entries_;
	///
	mutable std::atomic_bool should_refresh_ = {true};
	///
	std::uint64_t watch_id_ = 0;
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
