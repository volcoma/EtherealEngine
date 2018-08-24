#include <utility>

#include "filesystem_watcher.h"
namespace fs
{
using namespace std::literals;

static void log_path(const fs::path& /*unused*/)
{
}
static std::pair<path, std::string> get_path_filter_pair(const fs::path& path)
{
	// extract wild card and parent path
	std::string key = path.string();
	fs::path p = path;
	size_t wildCardPos = key.find('*');
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
														 bool visit_empty,
														 const std::function<bool(const fs::path&)>& visitor)
{
	std::pair<fs::path, std::string> path_filter = get_path_filter_pair(path);
	if(!path_filter.second.empty())
	{
		std::string full = (path_filter.first / path_filter.second).string();
		size_t wildcard_pos = full.find('*');
		std::string before = full.substr(0, wildcard_pos);
		std::string after = full.substr(wildcard_pos + 1);
		fs::directory_iterator end;
		fs::error_code err;
		if(visit_empty && fs::is_empty(path_filter.first, err))
		{
			visitor(path_filter.first);
		}
		else if(fs::exists(path_filter.first, err))
		{
			const auto iterate = [&](auto& it) {
				for(const auto& entry : it)
				{
					std::string current = entry.path().string();
					size_t before_pos = current.find(before);
					size_t after_pos = current.find(after);
					if((before_pos != std::string::npos || before.empty()) &&
					   (after_pos != std::string::npos || after.empty()))
					{
						if(visitor(entry.path()))
						{
							break;
						}
					}
				}
			};

			if(recursive)
			{
				fs::recursive_directory_iterator it(path_filter.first, err);
				iterate(it);
			}
			else
			{
				fs::directory_iterator it(path_filter.first, err);
				iterate(it);
			}
		}
	}
	return path_filter;
}

class filesystem_watcher::impl
{
public:
	//-----------------------------------------------------------------------------
	//  Name : impl ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	impl(const fs::path& path, const std::string& filter, bool recursive, bool initial_list,
		 clock_t::duration poll_interval, notify_callback list_callback)
		: filter_(filter)
		, callback_(std::move(list_callback))
		, poll_interval_(poll_interval)
		, recursive_(recursive)
	{
		root_ = path;
		std::vector<filesystem_watcher::entry> entries;
		std::vector<size_t> created;
		std::vector<size_t> modified;
		// make sure we store all initial write time
		if(!filter_.empty())
		{
			visit_wild_card_path(path / filter, recursive, false,
								 [this, &entries, &created, &modified](const fs::path& p) {
									 poll_entry(p, entries, created, modified);
									 return false;
								 });
		}
		else
		{
			poll_entry(root_, entries, created, modified);
		}

		if(initial_list)
		{
			// this means that the first watch won't call the callback function
			// so we have to manually call it here if we want that behavior
			if(!entries.empty() && callback_)
			{
				callback_(entries, true);
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
		if(!filter_.empty())
		{
			visit_wild_card_path(root_ / filter_, recursive_, false,
								 [this, &entries, &created, &modified](const fs::path& p) {
									 poll_entry(p, entries, created, modified);
									 return false;
								 });
		}
		else
		{
			poll_entry(root_, entries, created, modified);
		}

		process_modifications(entries, created, modified);

		if(!entries.empty() && callback_)
		{
			callback_(entries, false);
		}
	}

	void process_modifications(std::vector<filesystem_watcher::entry>& entries,
							   const std::vector<size_t>& created, const std::vector<size_t>& /*unused*/)
	{
		using namespace std::literals;

		auto it = std::begin(entries_);
		while(it != std::end(entries_))
		{
			auto& fi = it->second;
			fs::error_code err;
			if(!fs::exists(fi.path, err))
			{
				bool was_removed = true;
				for(auto idx : created)
				{
					auto& e = entries[idx];
					if(e.size == fi.size)
					{
						//using sys_clock = std::chrono::system_clock;
						//std::chrono::microseconds tolerance = 1000us;
						//auto diff = sys_clock::from_time_t(e.last_mod_time - fi.last_mod_time);
						//auto d = std::chrono::time_point_cast<std::chrono::microseconds>(diff);
						if(e.last_mod_time == fi.last_mod_time)
						{

							e.status = filesystem_watcher::entry_status::renamed;
							e.last_path = fi.path;
							was_removed = false;
							break;
						}
					}
				}

				if(was_removed)
				{
					fi.status = filesystem_watcher::entry_status::removed;
					entries.push_back(fi);
				}

				it = entries_.erase(it);
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
		auto it = entries_.find(key);
		if(it != entries_.end())
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
			auto& fi = entries_[key];
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
	fs::path root_;
	/// Filter applied
	std::string filter_;
	/// Callback for list of modifications
	notify_callback callback_;
	/// Cache watched files
	std::map<std::string, filesystem_watcher::entry> entries_;
	///
	clock_t::duration poll_interval_ = 500ms;

	clock_t::time_point last_poll_ = clock_t::now();
	///
	bool recursive_ = false;
};

static filesystem_watcher& get_watcher()
{
	// create the static filesystem_watcher instance
	static filesystem_watcher wd;
	return wd;
}

std::uint64_t filesystem_watcher::watch(const fs::path& path, bool recursive, bool initial_list,
										clock_t::duration poll_interval, notify_callback callback)
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
	if(path.string().find('*') != std::string::npos)
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
	watching_ = false;
	// remove all watchers
	unwatch_all();

	if(thread_.joinable())
	{
		thread_.join();
	}
}

void filesystem_watcher::start()
{
	watching_ = true;
	thread_ = std::thread([this]() {
		// keep watching for modifications every ms milliseconds
		using namespace std::literals;
		while(watching_)
		{
			clock_t::duration sleep_time = 99999h;

			// iterate through each watcher and check for modification
			std::map<std::uint64_t, std::shared_ptr<impl>> watchers;
			{
				std::unique_lock<std::mutex> lock(mutex_);
				watchers = watchers_;
			}

			for(auto& pair : watchers)
			{
				auto watcher = pair.second;

				auto now = clock_t::now();

				auto diff = (watcher->last_poll_ + watcher->poll_interval_) - now;
				if(diff <= clock_t::duration(0))
				{
					watcher->watch();
					watcher->last_poll_ = now;

					sleep_time = std::min(sleep_time, watcher->poll_interval_);
				}
				else
				{
					sleep_time = std::min(sleep_time, diff);
				}
			}

			std::unique_lock<std::mutex> lock(mutex_);
			cv_.wait_for(lock, sleep_time);
		}
	});
}

std::uint64_t filesystem_watcher::watch_impl(const fs::path& path, bool recursive, bool initial_list,
											 clock_t::duration poll_interval,
											 notify_callback& list_callback)
{
	auto& wd = get_watcher();
	// and start its thread
	if(!wd.watching_)
	{
		wd.start();
	}

	// add a new watcher
	if(list_callback)
	{
		std::string filter;
		fs::path p = path;
		// try to see if there's a match for the wild card
		if(path.string().find('*') != std::string::npos)
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

		static std::atomic<std::uint64_t> free_id = {1};

		auto key = free_id++;
		{
			// we do it like this because if initial_list is true we don't want
			// to call a user callback on a locked mutex
			auto imp =
				std::make_shared<impl>(p, filter, recursive, initial_list, poll_interval, std::move(list_callback));
			std::lock_guard<std::mutex> lock(wd.mutex_);
			wd.watchers_.emplace(key, std::move(imp));
		}
		wd.cv_.notify_all();
		return key;
	}

	return 0;
}

void filesystem_watcher::unwatch_impl(std::uint64_t key)
{
	auto& wd = get_watcher();

	{
		std::lock_guard<std::mutex> lock(wd.mutex_);
		wd.watchers_.erase(key);
	}
	wd.cv_.notify_all();
}

void filesystem_watcher::unwatch_all_impl()
{
	auto& wd = get_watcher();
	{
		std::lock_guard<std::mutex> lock(wd.mutex_);
		wd.watchers_.clear();
	}
	wd.cv_.notify_all();
}
}
