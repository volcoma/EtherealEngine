#pragma once

#include <map>
#include <string>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>

#include "filesystem.h"

//-----------------------------------------------------------------------------
//  Name : log_path ()
/// <summary>
/// When watcher can't locate a file or parse the wild card
/// </summary>
//-----------------------------------------------------------------------------
inline void log_path(const fs::path& path)
{
}

namespace fs
{

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

		//-----------------------------------------------------------------------------
		//  Name : watch ()
		/// <summary>
		/// Watches a file or directory for modification and call back the specified
		/// std::function. A list of modified files or directory is passed as argument 
		/// of the callback. Use this version only if you are watching multiple files 
		/// or a directory.
		/// </summary>
		//-----------------------------------------------------------------------------
		static void watch(const fs::path &path, bool initialList, const std::function<void(const std::vector<entry>&, bool)> &callback)
		{
			watch_impl(path, initialList, callback);
		}

		//-----------------------------------------------------------------------------
		//  Name : unwatch ()
		/// <summary>
		/// Un-watches a previously registered file or directory
		/// </summary>
		//-----------------------------------------------------------------------------
		static void unwatch(const fs::path &path, bool recursive = false)
		{
			unwatch_impl(path, recursive);
		}

		//-----------------------------------------------------------------------------
		//  Name : unwatch_all ()
		/// <summary>
		/// Un-watches all previously registered file or directory
		/// </summary>
		//-----------------------------------------------------------------------------
		static void unwatch_all()
		{
			unwatch_impl(fs::path());
		}

		//-----------------------------------------------------------------------------
		//  Name : touch ()
		/// <summary>
		/// Sets the last modification time of a file or directory. by default sets the time to the current time 
		/// </summary>
		//-----------------------------------------------------------------------------
		static void touch(const fs::path &path, fs::file_time_type time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
		{
            fs::error_code err;
			// if the file or directory exists change its last write time
            if (fs::exists(path, err))
			{
                fs::last_write_time(path, time, err);
				return;
			}
			// if not, visit each path if there's a wild card
			if (path.string().find("*") != std::string::npos)
			{
				visit_wild_card_path(path, true, [time](const fs::path &p)
				{
                    fs::error_code err;
                    fs::last_write_time(p, time, err);
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
		//  Name : ~filesystem_watcher ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		~filesystem_watcher()
		{
			close();
		}
	protected:

		//-----------------------------------------------------------------------------
		//  Name : filesystem_watcher ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		filesystem_watcher()
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
						std::lock_guard<std::recursive_mutex> lock(_mutex);
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

		static filesystem_watcher& get_watcher()
		{
			// create the static filesystem_watcher instance
			static filesystem_watcher wd;
			return wd;
		}

		//-----------------------------------------------------------------------------
		//  Name : watch_impl ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		static void watch_impl(const fs::path &path, bool initialList = true, const std::function<void(const std::vector<entry>&, bool)> &listCallback = {})
		{
			auto& wd = get_watcher();
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

					p = pathFilter.first;
					filter = pathFilter.second;
			
				}
				else
				{
                    fs::error_code err;
                    if (!fs::exists(path, err))
					{
						log_path(path);
						return;
					}
				}

				std::lock_guard<std::recursive_mutex> lock(wd._mutex);
				if (wd._watchers.find(key) == wd._watchers.end())
				{
					wd._watchers.emplace(make_pair(key, watcher_impl(p, filter, initialList, listCallback)));
				}
			}
			
		}

		static void unwatch_impl(const fs::path &path, bool recursive = false)
		{
			auto& wd = get_watcher();
			const std::string key = path.string();
			const fs::path dir = path.parent_path();

			// if the path is empty we unwatch all files
			if (path.empty())
			{
				std::lock_guard<std::recursive_mutex> lock(wd._mutex);
				wd._watchers.clear();
			}
			// or the specified file or directory
			else
			{
				std::lock_guard<std::recursive_mutex> lock(wd._mutex);

				if (recursive && wd._watchers.size() > 0)
				{
					for (auto it = wd._watchers.begin(); it != wd._watchers.end(); )
					{
                        auto watcher_key = fs::path(it->first).parent_path();
						if (watcher_key == dir)
						{
							it->second.watch();
							it = wd._watchers.erase(it);
						}
						else
							++it;
					}
				}
				else
				{
					auto watcher = wd._watchers.find(key);
					if (watcher != wd._watchers.end())
					{
						watcher->second.watch();
						wd._watchers.erase(watcher);
					}
				}
				
			}
		}

		//-----------------------------------------------------------------------------
		//  Name : get_path_filter_pair ()
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

            fs::error_code err;
            if (filter.empty() && !fs::exists(p, err))
			{
				log_path(path);
			}


			return std::make_pair(p, filter);

		}

		//-----------------------------------------------------------------------------
		//  Name : visit_wild_card_path ()
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
                fs::error_code err;
                if (visitEmpty && fs::is_empty(pathFilter.first, err))
				{
					visitor(pathFilter.first);
				}
				else if(fs::exists(pathFilter.first, err))
				{
					for (fs::directory_iterator it(pathFilter.first, err); it != end; ++it)
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

		static std::pair<fs::path, std::string> visit_wild_card_path_cache(const std::map <std::string, entry>& entries, const fs::path &path, bool visitEmpty, const std::function<bool(const fs::path&)> &visitor)
		{
			std::pair<fs::path, std::string> pathFilter = get_path_filter_pair(path);
			if (!pathFilter.second.empty())
			{
				std::string full = (pathFilter.first / pathFilter.second).string();
				size_t wildcardPos = full.find("*");
				std::string before = full.substr(0, wildcardPos);
				std::string after = full.substr(wildcardPos + 1);

				for (auto& it : entries)
				{
					std::string current = it.second.path.string();
					size_t beforePos = current.find(before);
					size_t afterPos = current.find(after);
					if ((beforePos != std::string::npos || before.empty())
						&& (afterPos != std::string::npos || after.empty()))
					{
						if (visitor(it.second.path))
						{
							break;
						}
					}
				}
			}
			return pathFilter;
		}

		class watcher_impl
		{
		public:
			//-----------------------------------------------------------------------------
			//  Name : watcher_impl ()
			/// <summary>
			/// 
			/// 
			/// 
			/// </summary>
			//-----------------------------------------------------------------------------
			watcher_impl(const fs::path &path, const std::string &filter, bool initialList, const std::function<void(const std::vector<entry>&, bool)> &listCallback)
				: _filter(filter), _callback(listCallback)
			{
				_root = path;
				std::vector<entry> entries;
				// make sure we store all initial write time
				if (!_filter.empty())
				{
					visit_wild_card_path(path / filter, false, [this, &entries](const fs::path &p)
					{
						entry e;
						poll_entry(p, e);
						entries.push_back(e);
						return false;
					});
				}
				else
				{
					entry e;
					poll_entry(_root, e);
					entries.push_back(e);
				}

				_entries_cached = _entries;

				if (initialList)
				{
					// this means that the first watch won't call the callback function
					// so we have to manually call it here if we want that behavior
					if (entries.size() > 0 && _callback)
					{
						_callback(entries, true);
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
				
				std::vector<entry> entries;
				// otherwise we check the whole parent directory
				if (!_filter.empty())
				{
					visit_wild_card_path(_root / _filter, false, [this, &entries](const fs::path &p)
					{
						entry e;
						poll_entry(p, e);
						if (e.status != entry_status::unmodified && _callback)
						{
							entries.push_back(e);
						}
						return false;
					});

					visit_wild_card_path_cache(_entries_cached, _root / _filter, false, [this, &entries](const fs::path &p)
					{
						entry e;
						poll_entry(p, e);
						if (e.status == entry_status::removed && _callback)
						{
							entries.push_back(e);
						}
						return false;
					});

				}
				else
				{
					entry e;
					poll_entry(_root, e);

					if (e.status != entry_status::unmodified && _callback)
					{
						entries.push_back(e);
					}
				}

				auto __entries = entries;
				for (auto& e : __entries)
				{
					if (e.status == entry_status::created)
					{
						for (auto& other : __entries)
						{
							if (other.status == entry_status::removed)
							{
								if (e.last_mod_time == other.last_mod_time && e.size == other.size)
								{
									entries.erase(std::remove_if(std::begin(entries), std::end(entries),
										[&other](const entry& rhs) { return other.path == rhs.path; }
									), std::end(entries));

									auto it = std::find_if(std::begin(entries), std::end(entries), 
										[&e](const entry& rhs) { return e.path == rhs.path; }
									);
									if (it != std::end(entries))
									{
										it->status = entry_status::renamed;
										it->last_path = other.path;
									}
								}

							}
						}
					}
				}

				_entries_cached = _entries;

				if (entries.size() > 0 && _callback)
				{
					_callback(entries, false);
				}
			}

			//-----------------------------------------------------------------------------
			//  Name : poll_entry ()
			/// <summary>
			/// 
			/// 
			/// 
			/// </summary>
			//-----------------------------------------------------------------------------
			void poll_entry(const fs::path &path, entry& entry)
			{
				// get the last modification time
                fs::error_code err;
                auto time = fs::last_write_time(path, err);
                auto size = fs::file_size(path, err);
                fs::file_status status = fs::status(path, err);
				// add a new modification time to the map
				std::string key = path.string();
				if (_entries.find(key) == _entries.end())
				{
					auto &fi = _entries[key];
					fi.path = path;
					fi.last_path = path;
					fi.last_mod_time = time;
					fi.status = entry_status::created;
					fi.size = size;
					fi.type = status.type();
					entry = fi;
					return;
				}
				// or compare with an older one
				auto &fi = _entries[key];
                if (!fs::exists(fi.path, err))
				{
					auto fi_copy = fi;
					fi_copy.status = entry_status::removed;
					_entries.erase(key);
					entry = fi_copy;
					return;
				}


				if (fi.last_mod_time != time || fi.size != size || fi.type != status.type())
				{
					fi.size = size;
					fi.last_mod_time = time;
					fi.status = entry_status::modified;
					fi.type = status.type();
					entry = fi;
					return;
				}
				else
				{
					fi.status = entry_status::unmodified;
					fi.type = status.type();
					entry = fi;
					return;
				}
			};

		protected:
			/// Path to watch
			fs::path _root;
			/// Filter applied
			std::string _filter;
			/// Callback for list of modifications
			std::function<void(const std::vector<entry>&, bool)> _callback;
			/// Cache watched files
			std::map <std::string, entry> _entries;
			/// Cache watched files
			std::map <std::string, entry> _entries_cached;
		};
		/// Mutex for the file watchers
		std::recursive_mutex _mutex;
		/// Atomic bool sync
		std::atomic<bool> _watching;
		/// Thread that polls for changes
		std::thread _thread;
		/// Registered file watchers
		std::map<std::string, watcher_impl> _watchers;
	};

	using watcher = filesystem_watcher;
}
