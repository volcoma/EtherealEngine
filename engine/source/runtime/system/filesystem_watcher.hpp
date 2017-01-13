
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
//  Name : log_path ()
/// <summary>
/// When watcher can't locate a file or parse the wild card
/// </summary>
//-----------------------------------------------------------------------------
inline void log_path(const fs::path& path)
{
	auto logger = logging::get("Log");
	logger->error() << (std::string("Watcher can't locate a file or parse the wild card at: ") + path.string());
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
			uintmax_t size;
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
			watch_impl(path, initialList, callback);
		}

		//-----------------------------------------------------------------------------
		//  Name : unwatch ()
		/// <summary>
		/// Un-watches a previously registered file or directory
		/// </summary>
		//-----------------------------------------------------------------------------
		static void unwatch(const fs::path &path)
		{
			watch_impl(path);
		}

		//-----------------------------------------------------------------------------
		//  Name : unwatch_all ()
		/// <summary>
		/// Un-watches all previously registered file or directory
		/// </summary>
		//-----------------------------------------------------------------------------
		static void unwatch_all()
		{
			watch_impl(fs::path());
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
				visit_wild_card_path(path, true, [time](const fs::path &p)
				{
					fs::last_write_time(p, time, std::error_code{});
					return false;
				});
			}
			// otherwise throw an exception
			else
			{
				log_path(path);
			}
		}

		//-----------------------------------------------------------------------------
		//  Name : ~FSWatcher ()
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
		//  Name : FSWatcher ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		FSWatcher()
			: _watching(false)
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
			unwatch_all();

			// stop the thread
			_watching = false;

			if (_thread.joinable())
				_thread.join();
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
			_watching = true;
			_thread = std::thread([this]()
			{
				// keep watching for modifications every ms milliseconds
				auto ms = std::chrono::milliseconds(500);
				while (_watching)
				{
					do
					{
						// iterate through each watcher and check for modification
						std::lock_guard<std::mutex> lock(_mutex);
						auto end = _watchers.end();
						for (auto it = _watchers.begin(); it != end; ++it)
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
		static void watch_impl(const fs::path &path, bool initialList = true, const std::function<void(const std::vector<Entry>&)> &listCallback = std::function<void(const std::vector<Entry>&)>())
		{
			// create the static FSWatcher instance
			static FSWatcher wd;
			// and start its thread
			if (!wd._watching)
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
					std::pair<fs::path, std::string> pathFilter = visit_wild_card_path(path, true, [&found](const fs::path &p)
					{
						found = true;
						return true;
					});
// 					if (!found)
// 					{
// 						log_path(path);
// 						return;
// 					}
// 					else
					{
						p = pathFilter.first;
						filter = pathFilter.second;
					}
				}
				else
				{
					if (!fs::exists(path, std::error_code{}))
					{
						log_path(path);
						return;
					}
				}

				std::lock_guard<std::mutex> lock(wd._mutex);
				if (wd._watchers.find(key) == wd._watchers.end())
				{
					wd._watchers.emplace(make_pair(key, Watcher(p, filter, initialList, listCallback)));
				}
			}
			// if there is no callback that means that we are un-watching
			else
			{
				// if the path is empty we unwatch all files
				if (path.empty())
				{
					std::lock_guard<std::mutex> lock(wd._mutex);
					for (auto it = wd._watchers.begin(); it != wd._watchers.end(); )
					{
						it = wd._watchers.erase(it);
					}
				}
				// or the specified file or directory
				else
				{
					std::lock_guard<std::mutex> lock(wd._mutex);
					auto watcher = wd._watchers.find(key);
					if (watcher != wd._watchers.end())
					{
						wd._watchers.erase(watcher);
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
		static std::pair<fs::path, std::string> get_path_filter_pair(const fs::path &path)
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
				log_path(path);
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
		static std::pair<fs::path, std::string> visit_wild_card_path(const fs::path &path, bool visitEmpty, const std::function<bool(const fs::path&)> &visitor)
		{
			std::pair<fs::path, std::string> pathFilter = get_path_filter_pair(path);
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
				: _filter(filter), _callback(listCallback)
			{

				_root = poll_entry(path);

				std::vector<Entry> entries;
				// make sure we store all initial write time
				if (!_filter.empty())
				{
					visit_wild_card_path(path / filter, false, [this, &entries](const fs::path &p)
					{
						auto entry = poll_entry(p);
						entries.push_back(entry);
						return false;
					});
				}
				else
				{
					entries.push_back(_root);
				}
				_entries_cached = _entries;
				if (initialList)
				{
					// this means that the first watch won't call the callback function
					// so we have to manually call it here if we want that behavior
					if (entries.size() > 0 && _callback)
					{
						_callback(entries);
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
				
				std::vector<Entry> entries;
				// otherwise we check the whole parent directory
				if (!_filter.empty())
				{
					visit_wild_card_path(_root.path / _filter, false, [this, &entries](const fs::path &p)
					{
						auto entry = poll_entry(p);
						if (entry.state == Entry::State::Modified && _callback)
						{
							entries.push_back(entry);
						}
						return false;
					});
					if (entries.size() > 0)
						touch(_root.path);

					_root = poll_entry(_root.path);

					if (_root.state != Entry::State::Unmodified)
					{
						for (auto& var : _entries_cached)
						{
							auto& entry = var.second;
							if (!fs::exists(entry.path, std::error_code{}))
							{
								entry.state = Entry::State::Removed;
								entries.push_back(entry);
								_entries.erase(var.first);
							}
						}
						_entries_cached = _entries;
					}
				}
				else
				{
					auto& entry = _root;
					if (!fs::exists(entry.path, std::error_code{}))
					{
						entry.state = Entry::State::Removed;
						_entries.erase(entry.path.string());
					}
					entries.push_back(_root);
					_entries_cached = _entries;
				}

				if (entries.size() > 0 && _callback)
				{
					_callback(entries);
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
			Entry& poll_entry(const fs::path &path)
			{
				// get the last modification time
				auto time = fs::last_write_time(path, std::error_code{});
				auto size = fs::file_size(path, std::error_code{});
				// add a new modification time to the map
				std::string key = path.string();
				if (_entries.find(key) == _entries.end())
				{
					auto &fi = _entries[key];
					fi.path = path;
					fi.last_mod_time = time;
					fi.state = Entry::State::Modified;
					fi.size = size;
					return fi;
				}
				// or compare with an older one
				auto &fi = _entries[key];
				if (fi.last_mod_time < time || fi.size != size)
				{
					fi.size = size;
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
			Entry _root;
			/// Filter applied
			std::string _filter;
			/// Callback for list of modifications
			std::function<void(const std::vector<Entry>&)> _callback;
			/// Cache watched files
			std::map <std::string, Entry> _entries;
			/// Cache watched files
			std::map <std::string, Entry> _entries_cached;
		};
		/// Mutex for the file watchers
		std::mutex _mutex;
		/// Atomic bool sync
		std::atomic<bool> _watching;
		/// Thread that polls for changes
		std::thread _thread;
		/// Registered file watchers
		std::map<std::string, Watcher> _watchers;
	};

	using watcher = FSWatcher;
}
