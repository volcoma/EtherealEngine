#include "filesystem_watcher.h"
namespace fs
{
using namespace std::literals;

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
	std::pair<fs::path, std::string> path_filter = get_path_filter_pair(path);
	if(!path_filter.second.empty())
	{
		std::string full = (path_filter.first / path_filter.second).string();
		size_t wildcardPos = full.find("*");
		std::string before = full.substr(0, wildcardPos);
		std::string after = full.substr(wildcardPos + 1);
		fs::directory_iterator end;
		fs::error_code err;
		if(visitEmpty && fs::is_empty(path_filter.first, err))
		{
			visitor(path_filter.first);
		}
		else if(fs::exists(path_filter.first, err))
		{
			for(fs::directory_iterator it(path_filter.first, err); it != end; ++it)
			{
				if(it->status().type() == file_type::directory_file && recursive)
				{
					visit_wild_card_path(it->path() / path_filter.second, recursive, visitEmpty, visitor);
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
	return path_filter;
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
	watcher_impl(const fs::path& path, const std::string& filter, bool recursive, bool initial_list,
				 clock_t::duration poll_interval, const notify_callback& list_callback)
		: _filter(filter)
		, _callback(list_callback)
		, _poll_interval(poll_interval)
		, _recursive(recursive)
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

		if(initial_list)
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
	friend class filesystem_watcher;
	/// Path to watch
	fs::path _root;
	/// Filter applied
	std::string _filter;
	/// Callback for list of modifications
	notify_callback _callback;
	/// Cache watched files
	std::map<std::string, filesystem_watcher::entry> _entries;
	///
	clock_t::duration _poll_interval = 500ms;

	clock_t::time_point _last_poll = clock_t::now();
	///
	bool _recursive = false;
};

static filesystem_watcher& get_watcher()
{
	// create the static filesystem_watcher instance
	static filesystem_watcher wd;
	return wd;
}

std::uint64_t filesystem_watcher::watch(const fs::path& path, bool recursive, bool initial_list,
										clock_t::duration poll_interval, const notify_callback& callback)
{
	return watch_impl(path, recursive, initial_list, poll_interval, callback);
}

void filesystem_watcher::unwatch(std::uint64_t key)
{
	unwatch_impl(key);
}

void filesystem_watcher::unwatch_all()
{
	unwatch_all_impl();
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
	// stop the thread
	_watching = false;
	// remove all watchers
	unwatch_all();

	if(_thread.joinable())
		_thread.join();
}

void filesystem_watcher::start()
{
	_watching = true;
	_thread = std::thread([this]() {
		// keep watching for modifications every ms milliseconds
		using namespace std::literals;
		while(_watching)
		{
			clock_t::duration sleep_time = 99999h;

			// iterate through each watcher and check for modification
			std::map<std::uint64_t, std::shared_ptr<watcher_impl>> watchers;
			{
				std::unique_lock<std::mutex> lock(_mutex);
				watchers = _watchers;
			}

			for(auto& pair : watchers)
			{
				auto watcher = pair.second;

				auto now = clock_t::now();

				auto diff = (watcher->_last_poll + watcher->_poll_interval) - now;
				if(diff <= clock_t::duration(0))
				{
					watcher->watch();
					watcher->_last_poll = now;

					sleep_time = std::min(sleep_time, watcher->_poll_interval);
				}
				else
				{
					sleep_time = std::min(sleep_time, diff);
				}
			}

			std::unique_lock<std::mutex> lock(_mutex);
			_cv.wait_for(lock, sleep_time);
		}
	});
}

std::uint64_t filesystem_watcher::watch_impl(const fs::path& path, bool recursive, bool initial_list,
											 clock_t::duration poll_interval,
											 const notify_callback& list_callback)
{
	auto& wd = get_watcher();
	// and start its thread
	if(!wd._watching)
		wd.start();

	// add a new watcher
	if(list_callback)
	{
		std::string filter;
		fs::path p = path;
		// try to see if there's a match for the wild card
		if(path.string().find("*") != std::string::npos)
		{
			std::pair<fs::path, std::string> path_filter =
				visit_wild_card_path(path, recursive, true, [](const fs::path&) { return true; });

			p = path_filter.first;
			filter = path_filter.second;
		}
		else
		{
			fs::error_code err;
			if(!fs::exists(path, err))
			{
				log_path(path);
				return 0;
			}
		}

		static std::atomic<std::uint64_t> s_id = {1};

		auto key = s_id++;
		{
			// we do it like this because if initial_list is true we don't want
			// to call a user callback on a locked mutex
			auto impl = std::make_shared<watcher_impl>(p, filter, recursive, initial_list, poll_interval,
													   list_callback);
			std::lock_guard<std::mutex> lock(wd._mutex);
			wd._watchers.emplace(key, std::move(impl));
		}
		wd._cv.notify_all();
		return key;
	}

	return 0;
}

void filesystem_watcher::unwatch_impl(std::uint64_t key)
{
	auto& wd = get_watcher();

	{
		std::lock_guard<std::mutex> lock(wd._mutex);
		wd._watchers.erase(key);
	}
	wd._cv.notify_all();
}

void filesystem_watcher::unwatch_all_impl()
{
	auto& wd = get_watcher();
	{
		std::lock_guard<std::mutex> lock(wd._mutex);
		wd._watchers.clear();
	}
	wd._cv.notify_all();
}
}
