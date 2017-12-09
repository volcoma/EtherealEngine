#include "filesystem_watcher.h"
namespace fs
{
static void log_path(const fs::path&)
{
}
static std::pair<path, std::string> get_path_filter_pair(const fs::path& path)
{
	// extract wild card and parent path
	std::string key = path.string();
	fs::path p = path;
	size_t wildCardPos = key.find("*");
	std::string filter;
	if(wildCardPos != std::string::npos)
	{
		filter = path.filename().string();
		p = path.parent_path();
	}

	fs::error_code err;
	if(filter.empty() && !fs::exists(p, err))
	{
		log_path(path);
	}

	return std::make_pair(p, filter);
}

static std::pair<path, std::string> visit_wild_card_path(const fs::path& path, bool recursive,
														 bool visitEmpty,
														 const std::function<bool(const fs::path&)>& visitor)
{
	std::pair<fs::path, std::string> pathFilter = get_path_filter_pair(path);
	if(!pathFilter.second.empty())
	{
		std::string full = (pathFilter.first / pathFilter.second).string();
		size_t wildcardPos = full.find("*");
		std::string before = full.substr(0, wildcardPos);
		std::string after = full.substr(wildcardPos + 1);
		fs::directory_iterator end;
		fs::error_code err;
		if(visitEmpty && fs::is_empty(pathFilter.first, err))
		{
			visitor(pathFilter.first);
		}
		else if(fs::exists(pathFilter.first, err))
		{
			for(fs::directory_iterator it(pathFilter.first, err); it != end; ++it)
			{
				if(it->status().type() == file_type::directory_file && recursive)
				{
					visit_wild_card_path(it->path() / pathFilter.second, recursive, visitEmpty, visitor);
				}

				std::string current = it->path().string();
				size_t beforePos = current.find(before);
				size_t afterPos = current.find(after);
				if((beforePos != std::string::npos || before.empty()) &&
				   (afterPos != std::string::npos || after.empty()))
				{
					if(visitor(it->path()))
					{
						break;
					}
				}
			}
		}
	}
	return pathFilter;
}

class filesystem_watcher::watcher_impl
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
	watcher_impl(const fs::path& path, const std::string& filter, bool recursive, bool initialList,
				 const notify_callback& listCallback)
		: _recursive(recursive)
		, _filter(filter)
		, _callback(listCallback)
	{
		_root = path;
		std::vector<filesystem_watcher::entry> entries;
		std::vector<size_t> created;
		std::vector<size_t> modified;
		// make sure we store all initial write time
		if(!_filter.empty())
		{
			visit_wild_card_path(path / filter, recursive, false,
								 [this, &entries, &created, &modified](const fs::path& p) {
									 poll_entry(p, entries, created, modified);
									 return false;
								 });
		}
		else
		{
			poll_entry(_root, entries, created, modified);
		}

		if(initialList)
		{
			// this means that the first watch won't call the callback function
			// so we have to manually call it here if we want that behavior
			if(entries.size() > 0 && _callback)
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

		std::vector<filesystem_watcher::entry> entries;
		std::vector<size_t> created;
		std::vector<size_t> modified;
		// otherwise we check the whole parent directory
		if(!_filter.empty())
		{
			visit_wild_card_path(_root / _filter, _recursive, false,
								 [this, &entries, &created, &modified](const fs::path& p) {
									 poll_entry(p, entries, created, modified);
									 return false;
								 });
		}
		else
		{
			poll_entry(_root, entries, created, modified);
		}

		process_modifications(entries, created, modified);

		if(entries.size() > 0 && _callback)
		{
			_callback(entries, false);
		}
	}

	void process_modifications(std::vector<filesystem_watcher::entry>& entries,
							   const std::vector<size_t>& created, const std::vector<size_t>&)
	{
		auto it = std::begin(_entries);
		while(it != std::end(_entries))
		{
			auto& fi = it->second;
			fs::error_code err;
			if(!fs::exists(fi.path, err))
			{
				bool was_removed = true;
				for(auto idx : created)
				{
					auto& e = entries[idx];
					if(e.last_mod_time == fi.last_mod_time && e.size == fi.size)
					{
						e.status = filesystem_watcher::entry_status::renamed;
						e.last_path = fi.path;
						was_removed = false;
						break;
					}
				}

				if(was_removed)
				{
					fi.status = filesystem_watcher::entry_status::removed;
					entries.push_back(fi);
				}

				it = _entries.erase(it);
			}
			else
			{
				it++;
			}
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
	void poll_entry(const fs::path& path, std::vector<filesystem_watcher::entry>& modifications,
					std::vector<size_t>& created, std::vector<size_t>& modified)
	{
		// get the last modification time
		fs::error_code err;
		auto time = fs::last_write_time(path, err);
		auto size = fs::file_size(path, err);
		fs::file_status status = fs::status(path, err);
		// add a new modification time to the map
		std::string key = path.string();
		auto it = _entries.find(key);
		if(it != _entries.end())
		{
			auto& fi = it->second;

			if(fi.last_mod_time != time || fi.size != size || fi.type != status.type())
			{
				fi.size = size;
				fi.last_mod_time = time;
				fi.status = filesystem_watcher::entry_status::modified;
				fi.type = status.type();
				modifications.push_back(fi);
				modified.push_back(modifications.size() - 1);
			}
			else
			{
				fi.status = filesystem_watcher::entry_status::unmodified;
				fi.type = status.type();
			}
		}
		else
		{
			// or compare with an older one
			auto& fi = _entries[key];
			fi.path = path;
			fi.last_path = path;
			fi.last_mod_time = time;
			fi.status = filesystem_watcher::entry_status::created;
			fi.size = size;
			fi.type = status.type();

			modifications.push_back(fi);
			created.push_back(modifications.size() - 1);
		}
	}

protected:
	bool _recursive = false;
	/// Path to watch
	fs::path _root;
	/// Filter applied
	std::string _filter;
	/// Callback for list of modifications
	notify_callback _callback;
	/// Cache watched files
	std::map<std::string, filesystem_watcher::entry> _entries;
};

static filesystem_watcher& get_watcher()
{
	// create the static filesystem_watcher instance
	static filesystem_watcher wd;
	return wd;
}

void filesystem_watcher::watch(const fs::path& path, bool recursive, bool initialList,
							   const notify_callback& callback)
{
	watch_impl(path, recursive, initialList, callback);
}

void filesystem_watcher::unwatch(const fs::path& path, bool recursive)
{
	unwatch_impl(path, recursive);
}

void filesystem_watcher::unwatch_all()
{
	unwatch_impl(fs::path());
}

void filesystem_watcher::touch(const fs::path& path, bool recursive, fs::file_time_type time)
{
	fs::error_code err;
	// if the file or directory exists change its last write time
	if(fs::exists(path, err))
	{
		fs::last_write_time(path, time, err);
		return;
	}
	// if not, visit each path if there's a wild card
	if(path.string().find("*") != std::string::npos)
	{
		visit_wild_card_path(path, recursive, true, [time](const fs::path& p) {
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

filesystem_watcher::~filesystem_watcher()
{
	close();
}

void filesystem_watcher::close()
{
	// remove all watchers
	unwatch_all();

	// stop the thread
	_watching = false;

	if(_thread.joinable())
		_thread.join();
}

void filesystem_watcher::start()
{
	_watching = true;
	_thread = std::thread([this]() {
		// keep watching for modifications every ms milliseconds
		using namespace std::literals;
		const auto poll_interval = 500ms;
		while(_watching)
		{
			do
			{
				// iterate through each watcher and check for modification
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				auto end = _watchers.end();
				for(auto it = _watchers.begin(); it != end; ++it)
				{
					it->second->watch();
				}
				// lock will be released before this thread goes to sleep
			} while(false);
			// make this thread sleep for a while
			std::this_thread::sleep_for(poll_interval);
		}
	});
}

void filesystem_watcher::watch_impl(const fs::path& path, bool recursive, bool initialList,
									const notify_callback& listCallback)
{
	auto& wd = get_watcher();
	// and start its thread
	if(!wd._watching)
		wd.start();

	const std::string key = path.string();

	// add a new watcher
	if(listCallback)
	{
		std::string filter;
		fs::path p = path;
		// try to see if there's a match for the wild card
		if(path.string().find("*") != std::string::npos)
		{
			std::pair<fs::path, std::string> pathFilter =
				visit_wild_card_path(path, recursive, true, [](const fs::path& p) { return true; });

			p = pathFilter.first;
			filter = pathFilter.second;
		}
		else
		{
			fs::error_code err;
			if(!fs::exists(path, err))
			{
				log_path(path);
				return;
			}
		}

		std::lock_guard<std::recursive_mutex> lock(wd._mutex);
		if(wd._watchers.find(key) == wd._watchers.end())
		{
			wd._watchers.emplace(make_pair(
				key, std::make_unique<watcher_impl>(p, filter, recursive, initialList, listCallback)));
		}
	}
}

void filesystem_watcher::unwatch_impl(const fs::path& path, bool recursive)
{
	auto& wd = get_watcher();
	const std::string key = path.string();
	const fs::path dir = path.parent_path();

	// if the path is empty we unwatch all files
	if(path.empty())
	{
		std::lock_guard<std::recursive_mutex> lock(wd._mutex);
		wd._watchers.clear();
	}
	// or the specified file or directory
	else
	{
		std::lock_guard<std::recursive_mutex> lock(wd._mutex);

		if(recursive && wd._watchers.size() > 0)
		{
			for(auto it = wd._watchers.begin(); it != wd._watchers.end();)
			{
				auto watcher_key = fs::path(it->first).parent_path();
				if(watcher_key == dir)
				{
					it->second->watch();
					it = wd._watchers.erase(it);
				}
				else
					++it;
			}
		}
		else
		{
			auto watcher = wd._watchers.find(key);
			if(watcher != wd._watchers.end())
			{
				watcher->second->watch();
				wd._watchers.erase(watcher);
			}
		}
	}
}
}
