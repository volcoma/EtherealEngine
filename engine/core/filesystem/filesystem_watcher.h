#ifndef FS_WATCHER_H
#define FS_WATCHER_H

#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <condition_variable>

#include "filesystem.h"

namespace fs
{
class filesystem_watcher;
using watcher = filesystem_watcher;

class filesystem_watcher
{
public:
	enum entry_status
	{
		created,
		modified,
		removed,
		renamed,
		unmodified,
	};

	struct entry
	{

		fs::path path;
		fs::path last_path;
		entry_status status = unmodified;
		fs::file_time_type last_mod_time;
		uintmax_t size = 0;
		fs::file_type type;
	};

	using notify_callback = std::function<void(const std::vector<entry>&, bool)>;
	using clock_t = std::chrono::steady_clock;
	//-----------------------------------------------------------------------------
	//  Name : watch ()
	/// <summary>
	/// Watches a file or directory for modification and call back the specified
	/// std::function. A list of modified files or directory is passed as argument
	/// of the callback. Use this version only if you are watching multiple files
	/// or a directory.
	/// </summary>
	//-----------------------------------------------------------------------------
	static std::uint64_t watch(const fs::path& path, bool recursive, bool initial_list,
							   clock_t::duration poll_interval, const notify_callback& callback);

	//-----------------------------------------------------------------------------
	//  Name : unwatch ()
	/// <summary>
	/// Un-watches a previously registered file or directory
	/// </summary>
	//-----------------------------------------------------------------------------
	static void unwatch(std::uint64_t key);

	//-----------------------------------------------------------------------------
	//  Name : unwatch_all ()
	/// <summary>
	/// Un-watches all previously registered file or directory
	/// </summary>
	//-----------------------------------------------------------------------------
	static void unwatch_all();

	//-----------------------------------------------------------------------------
	//  Name : touch ()
	/// <summary>
	/// Sets the last modification time of a file or directory. by default sets
	/// the time to the current time
	/// </summary>
	//-----------------------------------------------------------------------------
	static void
	touch(const fs::path& path, bool recursive,
		  fs::file_time_type time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

	//-----------------------------------------------------------------------------
	//  Name : ~filesystem_watcher ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	~filesystem_watcher();
	filesystem_watcher() = default;

protected:
	//-----------------------------------------------------------------------------
	//  Name : close ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void close();

	//-----------------------------------------------------------------------------
	//  Name : start ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void start();

	//-----------------------------------------------------------------------------
	//  Name : watch_impl ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	static std::uint64_t watch_impl(const fs::path& path, bool recursive, bool initialList ,
									clock_t::duration poll_interval, const notify_callback& listCallback);

	static void unwatch_impl(std::uint64_t key);

	static void unwatch_all_impl();

	/// Mutex for the file watchers
	std::mutex _mutex;
	/// Atomic bool sync
	std::atomic<bool> _watching = {false};
	
	std::condition_variable _cv;
	/// Thread that polls for changes
	std::thread _thread;
	/// Registered file watchers
	class watcher_impl;
	std::map<std::uint64_t, std::unique_ptr<watcher_impl>> _watchers;
};
}

#endif
