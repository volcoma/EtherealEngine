
#pragma once

#include <map>
#include <string>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>

#include "filesystem.h"
#include "core/logging/logging.h"

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

namespace fs
{

	class FSWatcher
	{
	public:
		struct Entry
		{
			enum State
			{
				Modified,
				Removed,
				Unmodified,
			};
			fs::path path;
			State state;
			fs::file_time_type last_mod_time;
		};

		//-----------------------------------------------------------------------------
		//  Name : watch ()
		/// <summary>
		/// Watches a file or directory for modification and call back the specified
		/// std::function. A list of modified files or directory is passed as argument 
		/// of the callback. Use this version only if you are watching multiple files 
		/// or a directory.
		/// </summary>
		//-----------------------------------------------------------------------------
		static void watch(const fs::path &path, bool initialList, const std::function<void(const std::vector<Entry>&)> &callback)
		{
			watchImpl(path, initialList, callback);
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
			if (fs::exists(path, std::error_code{}))
			{
				fs::last_write_time(path, time, std::error_code{});
				return;
			}
			// if not, visit each path if there's a wild card
			if (path.string().find("*") != std::string::npos)
			{
				visitWildCardPath(path, true, [time](const fs::path &p)
				{
					fs::last_write_time(p, time, std::error_code{});
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
		~FSWatcher()
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
		FSWatcher()
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
						auto end = mWatchers.end();
						for (auto it = mWatchers.begin(); it != end; ++it)
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
		static void watchImpl(const fs::path &path, bool initialList = true, const std::function<void(const std::vector<Entry>&)> &listCallback = std::function<void(const std::vector<Entry>&)>())
		{
			// create the static Watchdog instance
			static FSWatcher wd;
			// and start its thread
			if (!wd.mWatching)
				wd.start();

			const std::string key = path.string();

			// add a new watcher
			if (listCallback)
			{
				std::string filter;
				fs::path p = path;
				// try to see if there's a match for the wild card
				if (path.string().find("*") != std::string::npos)
				{
					bool found = false;
					std::pair<fs::path, std::string> pathFilter = visitWildCardPath(path, true, [&found](const fs::path &p)
					{
						found = true;
						return true;
					});
					if (!found)
					{
						logPath(path);
						return;
					}
					else
					{
						p = pathFilter.first;
						filter = pathFilter.second;
					}
				}
				else
				{
					if (!fs::exists(path, std::error_code{}))
					{
						logPath(path);
						return;
					}
				}

				std::lock_guard<std::mutex> lock(wd.mMutex);
				if (wd.mWatchers.find(key) == wd.mWatchers.end())
				{
					wd.mWatchers.emplace(make_pair(key, Watcher(p, filter, initialList, listCallback)));
				}
			}
			// if there is no callback that means that we are un-watching
			else
			{
				// if the path is empty we unwatch all files
				if (path.empty())
				{
					std::lock_guard<std::mutex> lock(wd.mMutex);
					for (auto it = wd.mWatchers.begin(); it != wd.mWatchers.end(); )
					{
						it = wd.mWatchers.erase(it);
					}
				}
				// or the specified file or directory
				else
				{
					std::lock_guard<std::mutex> lock(wd.mMutex);
					auto watcher = wd.mWatchers.find(key);
					if (watcher != wd.mWatchers.end())
					{
						wd.mWatchers.erase(watcher);
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
			if (filter.empty() && !fs::exists(p, std::error_code{}))
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
		static std::pair<fs::path, std::string> visitWildCardPath(const fs::path &path, bool visitEmpty, const std::function<bool(const fs::path&)> &visitor)
		{
			std::pair<fs::path, std::string> pathFilter = getPathFilterPair(path);
			if (!pathFilter.second.empty())
			{
				std::string full = (pathFilter.first / pathFilter.second).string();
				size_t wildcardPos = full.find("*");
				std::string before = full.substr(0, wildcardPos);
				std::string after = full.substr(wildcardPos + 1);
				fs::directory_iterator end;
				if (visitEmpty && fs::is_empty(pathFilter.first, std::error_code{}))
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
			Watcher(const fs::path &path, const std::string &filter, bool initialList, const std::function<void(const std::vector<Entry>&)> &listCallback)
				: mFilter(filter), mListCallback(listCallback)
			{

				mRoot = pollEntry(path);

				std::vector<Entry> entries;
				// make sure we store all initial write time
				if (!mFilter.empty())
				{
					visitWildCardPath(path / filter, false, [this, &entries](const fs::path &p)
					{
						auto entry = pollEntry(p);
						entries.push_back(entry);
						return false;
					});
				}
				else
				{
					entries.push_back(mRoot);
				}
				mLastEntries = mEntries;
				if (initialList)
				{
					// this means that the first watch won't call the callback function
					// so we have to manually call it here if we want that behavior
					if (mListCallback)
					{
						mListCallback(entries);
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
				mRoot = pollEntry(mRoot.path);
				std::vector<Entry> entries;
				// otherwise we check the whole parent directory
				if (!mFilter.empty())
				{
					visitWildCardPath(mRoot.path / mFilter, false, [this, &entries](const fs::path &p)
					{
						auto entry = pollEntry(p);
						if (entry.state == Entry::State::Modified && mListCallback)
						{
							entries.push_back(entry);
						}
						return false;
					});
					if (mRoot.state != Entry::State::Unmodified)
					{
						for (auto& var : mLastEntries)
						{
							auto& entry = var.second;
							if (!fs::exists(entry.path, std::error_code{}))
							{
								entry.state = Entry::State::Removed;
								entries.push_back(entry);
								mEntries.erase(var.first);
							}
						}
						mLastEntries = mEntries;
					}
				}
				else
				{
					auto& Entry = mRoot;
					if (!fs::exists(Entry.path, std::error_code{}))
					{
						Entry.state = Entry::State::Removed;
						mEntries.erase(Entry.path.string());
					}
					entries.push_back(mRoot);
					mLastEntries = mEntries;
				}

				if (entries.size() > 0 && mListCallback)
				{
					mListCallback(entries);
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
			Entry& pollEntry(const fs::path &path)
			{
				// get the last modification time
				auto time = fs::last_write_time(path, std::error_code{});
				// add a new modification time to the map
				std::string key = path.string();
				if (mEntries.find(key) == mEntries.end())
				{
					auto &fi = mEntries[key];
					fi.path = path;
					fi.last_mod_time = time;
					fi.state = Entry::State::Modified;
					return fi;
				}
				// or compare with an older one
				auto &fi = mEntries[key];
				if (fi.last_mod_time < time)
				{
					fi.last_mod_time = time;
					fi.state = Entry::State::Modified;
					return fi;
				}
				else
				{
					fi.state = Entry::State::Unmodified;
					return fi;
				}
			};

		protected:
			/// Path to watch
			Entry mRoot;
			/// Filter applied
			std::string mFilter;
			/// Callback for list of modifications
			std::function<void(const std::vector<Entry>&)> mListCallback;
			/// Cache watched files
			std::map <std::string, Entry> mEntries;
			/// Cache watched files
			std::map <std::string, Entry> mLastEntries;
		};
		/// Mutex for the file watchers
		std::mutex mMutex;
		/// Atomic bool sync
		std::atomic<bool> mWatching;
		/// Thread that polls for changes
		std::thread mThread;
		/// Registered file watchers
		std::map<std::string, Watcher> mWatchers;
	};

	using watcher = FSWatcher;
}
