
#pragma once

#include <map>
#include <string>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>

#include "FileSystem.h"
#include "Core/logging/logging.h"

//-----------------------------------------------------------------------------
//  Name : logPath ()
/// <summary>
/// When Watchdog can't locate a file or parse the wild card
/// </summary>
//-----------------------------------------------------------------------------
inline void logPath(const fs::path& path)
{
	auto logger = logging::get("Log");
	logger->error() << (std::string("Watchdog can't locate a file or parse the wild card at: ") + path.string());
}


class Watchdog
{
public:

	//-----------------------------------------------------------------------------
	//  Name : watch ()
	/// <summary>
	/// Watches a file or directory for modification and call back the specified 
	/// std::function. The path specified is passed as argument of the callback 
	/// even if there is multiple files. Use the second watch method if you want
	/// to receive a list of all the files that have been modified.
	/// </summary>
	//-----------------------------------------------------------------------------
	static void watch(const fs::path &path, const std::function<void(const fs::path&)> &callback)
	{
		watchImpl(path, callback, std::function<void(const std::vector<fs::path>&)>());
	}

	//-----------------------------------------------------------------------------
	//  Name : watch ()
	/// <summary>
	/// Watches a file or directory for modification and call back the specified
	/// std::function. A list of modified files or directory is passed as argument 
	/// of the callback. Use this version only if you are watching multiple files 
	/// or a directory.
	/// </summary>
	//-----------------------------------------------------------------------------
	static void watch(const fs::path &path, const std::function<void(const std::vector<fs::path>&)> &callback)
	{
		watchImpl(path, std::function<void(const fs::path&)>(), callback);
	}

	//-----------------------------------------------------------------------------
	//  Name : unwatch ()
	/// <summary>
	/// Un-watches a previously registered file or directory
	/// </summary>
	//-----------------------------------------------------------------------------
	static void unwatch(const fs::path &path)
	{
		watchImpl(path);
	}

	//-----------------------------------------------------------------------------
	//  Name : unwatchAll ()
	/// <summary>
	/// Un-watches all previously registered file or directory
	/// </summary>
	//-----------------------------------------------------------------------------
	static void unwatchAll()
	{
		watchImpl(fs::path());
	}

	//-----------------------------------------------------------------------------
	//  Name : touch ()
	/// <summary>
	/// Sets the last modification time of a file or directory. by default sets the time to the current time 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void touch(const fs::path &path, fs::file_time_type time = fs::file_time_type::clock::now())
	{

		// if the file or directory exists change its last write time
		if (fs::exists(path))
		{
			fs::last_write_time(path, time);
			return;
		}
		// if not, visit each path if there's a wild card
		if (path.string().find("*") != std::string::npos)
		{
			visitWildCardPath(path, [time](const fs::path &p)
			{
				fs::last_write_time(p, time);
				return false;
			});
		}
		// otherwise throw an exception
		else
		{
			logPath(path);
		}
	}

	//-----------------------------------------------------------------------------
	//  Name : ~Watchdog ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~Watchdog()
	{
		close();
	}
protected:

	//-----------------------------------------------------------------------------
	//  Name : Watchdog ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	Watchdog()
		: mWatching(false)
	{
	}


	//-----------------------------------------------------------------------------
	//  Name : close ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void close()
	{
		// remove all watchers
		unwatchAll();

		// stop the thread
		mWatching = false;

		if (mThread.joinable())
			mThread.join();
	}

	//-----------------------------------------------------------------------------
	//  Name : start ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void start()
	{
		mWatching = true;
		mThread = std::thread([this]()
		{
			// keep watching for modifications every ms milliseconds
			auto ms = std::chrono::milliseconds(500);
			while (mWatching)
			{
				do
				{
					// iterate through each watcher and check for modification
					std::lock_guard<std::mutex> lock(mMutex);
					auto end = mFileWatchers.end();
					for (auto it = mFileWatchers.begin(); it != end; ++it)
					{
						it->second.watch();
					}
					// lock will be released before this thread goes to sleep
				} while (false);

				// make this thread sleep for a while
				std::this_thread::sleep_for(ms);
			}
		});
	}

	//-----------------------------------------------------------------------------
	//  Name : watchImpl ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void watchImpl(const fs::path &path, const std::function<void(const fs::path&)> &callback = std::function<void(const fs::path&)>(), const std::function<void(const std::vector<fs::path>&)> &listCallback = std::function<void(const std::vector<fs::path>&)>())
	{
		// create the static Watchdog instance
		static Watchdog wd;
		// and start its thread
		if (!wd.mWatching)
			wd.start();

		const std::string key = path.string();

		// add a new watcher
		if (callback || listCallback) {

			std::string filter;
			fs::path p = path;
			// try to see if there's a match for the wild card
			if (path.string().find("*") != std::string::npos)
			{
				bool found = false;
				std::pair<fs::path, std::string> pathFilter = visitWildCardPath(path, [&found](const fs::path &p)
				{
					found = true;
					return true;
				});
				if (!found)
				{
					logPath(path);
				}
				else
				{
					p = pathFilter.first;
					filter = pathFilter.second;
				}
			}


			std::lock_guard<std::mutex> lock(wd.mMutex);
			if (wd.mFileWatchers.find(key) == wd.mFileWatchers.end())
			{
				wd.mFileWatchers.emplace(make_pair(key, Watcher(p, filter, callback, listCallback)));
			}
		}
		// if there is no callback that means that we are unwatching
		else
		{
			// if the path is empty we unwatch all files
			if (path.empty())
			{
				std::lock_guard<std::mutex> lock(wd.mMutex);
				for (auto it = wd.mFileWatchers.begin(); it != wd.mFileWatchers.end(); )
				{
					it = wd.mFileWatchers.erase(it);
				}
			}
			// or the specified file or directory
			else
			{
				std::lock_guard<std::mutex> lock(wd.mMutex);
				auto watcher = wd.mFileWatchers.find(key);
				if (watcher != wd.mFileWatchers.end())
				{
					wd.mFileWatchers.erase(watcher);
				}
			}
		}
	}

	//-----------------------------------------------------------------------------
	//  Name : getPathFilterPair ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static std::pair<fs::path, std::string> getPathFilterPair(const fs::path &path)
	{
		// extract wild card and parent path
		std::string key = path.string();
		fs::path p = path;
		size_t wildCardPos = key.find("*");
		std::string filter;
		if (wildCardPos != std::string::npos)
		{
			filter = path.filename().string();
			p = path.parent_path();
		}

		// throw an exception if the file doesn't exist
		if (filter.empty() && !fs::exists(p))
		{
			logPath(path);
		}


		return std::make_pair(p, filter);

	}

	//-----------------------------------------------------------------------------
	//  Name : visitWildCardPath ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static std::pair<fs::path, std::string> visitWildCardPath(const fs::path &path, const std::function<bool(const fs::path&)> &visitor)
	{
		std::pair<fs::path, std::string> pathFilter = getPathFilterPair(path);
		if (!pathFilter.second.empty())
		{
			std::string full = (pathFilter.first / pathFilter.second).string();
			size_t wildcardPos = full.find("*");
			std::string before = full.substr(0, wildcardPos);
			std::string after = full.substr(wildcardPos + 1);
			fs::directory_iterator end;
			if (fs::is_empty(pathFilter.first))
			{
				visitor(pathFilter.first);
			}
			else
			{
				for (fs::directory_iterator it(pathFilter.first); it != end; ++it)
				{
					std::string current = it->path().string();
					size_t beforePos = current.find(before);
					size_t afterPos = current.find(after);
					if ((beforePos != std::string::npos || before.empty())
						&& (afterPos != std::string::npos || after.empty()))
					{
						if (visitor(it->path()))
						{
							break;
						}
					}
				}
			}

		}
		return pathFilter;
	}

	class Watcher
	{
	public:
		//-----------------------------------------------------------------------------
		//  Name : Watcher ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		Watcher(const fs::path &path, const std::string &filter, const std::function<void(const fs::path&)> &callback, const std::function<void(const std::vector<fs::path>&)> &listCallback)
			: mPath(path), mFilter(filter), mCallback(callback), mListCallback(listCallback)
		{
			// make sure we store all initial write time
			if (!mFilter.empty())
			{
				std::vector<fs::path> paths;
				visitWildCardPath(path / filter, [this, &paths](const fs::path &p)
				{
					hasChanged(p);
					paths.push_back(p);
					return false;
				});
				// this means that the first watch won't call the callback function
				// so we have to manually call it here
				if (mCallback)
				{
					mCallback(mPath / mFilter);
				}
				else
				{
					mListCallback(paths);
				}
			}
		}

		//-----------------------------------------------------------------------------
		//  Name : watch ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void watch()
		{
			// if there's no filter we just check for one item
			if (mFilter.empty() && hasChanged(mPath) && mCallback)
				mCallback(mPath);

			// otherwise we check the whole parent directory
			else if (!mFilter.empty())
			{

				std::vector<fs::path> paths;
				visitWildCardPath(mPath / mFilter, [this, &paths](const fs::path &p)
				{
					bool pathHasChanged = hasChanged(p);
					if (pathHasChanged && mCallback)
					{
						mCallback(mPath / mFilter);
						return true;
					}
					else if (pathHasChanged && mListCallback)
					{
						paths.push_back(p);
					}
					return false;
				});
				if (paths.size() && mListCallback)
				{
					mListCallback(paths);
				}
			}

		}

		//-----------------------------------------------------------------------------
		//  Name : hasChanged ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool hasChanged(const fs::path &path)
		{
			// get the last modification time
			auto time = fs::last_write_time(path);
			// add a new modification time to the map
			std::string key = path.string();
			if (mModificationTimes.find(key) == mModificationTimes.end())
			{
				mModificationTimes[key] = time;
				return true;
			}
			// or compare with an older one
			auto &prev = mModificationTimes[key];
			if (prev < time)
			{
				prev = time;
				return true;
			}
			return false;
		};

	protected:
		/// Path to watch
		fs::path mPath;
		/// Filter applied
		std::string mFilter;
		/// Callback for single modification
		std::function<void(const fs::path&)> mCallback;
		/// Callback for list of modifications
		std::function<void(const std::vector<fs::path>&)> mListCallback;
		/// Cache modification times
		std::map< std::string, fs::file_time_type > mModificationTimes;
	};
	/// Mutex for the file watchers
	std::mutex mMutex;
	/// Atomic bool sync
	std::atomic<bool> mWatching;
	/// Thread that polls for changes
	std::thread mThread;
	/// Registered file watchers
	std::map<std::string, Watcher> mFileWatchers;
};

typedef Watchdog wd;