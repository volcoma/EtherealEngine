#include "filesystem_syncer.h"
#include "filesystem_watcher.h"

namespace fs
{
static void ensure_directory_exists(const fs::path& path)
{
	fs::error_code err;
	if(path.has_extension())
	{
		fs::error_code err;
		fs::create_directories(fs::path(path).parent_path(), err);
	}
	else
	{
		fs::error_code err;
		fs::create_directories(path, err);
	}
}

syncer::~syncer()
{
	unsync();
}

void syncer::set_mapping(const std::string& ref_ext, const std::vector<std::string>& synced_ext,
						 on_entry_created_t on_entry_created = nullptr,
						 on_entry_modified_t on_entry_modified = nullptr,
						 on_entry_removed_t on_entry_removed = nullptr,
						 on_entry_renamed_t on_entry_renamed = nullptr)
{
	std::lock_guard<std::mutex> lock(_mutex);
	auto& mapping = _mapping[ref_ext];
	mapping.extensions = synced_ext;
	mapping.on_entry_created = std::move(on_entry_created);
	mapping.on_entry_modified = std::move(on_entry_modified);
	mapping.on_entry_removed = std::move(on_entry_removed);
	mapping.on_entry_renamed = std::move(on_entry_renamed);
}

void syncer::set_directory_mapping(syncer::on_entry_created_t on_entry_created,
							 syncer::on_entry_modified_t on_entry_modified,
							 syncer::on_entry_removed_t on_entry_removed,
							 syncer::on_entry_renamed_t on_entry_renamed)
{
	std::lock_guard<std::mutex> lock(_mutex);
	auto& mapping = _mapping[""];
	mapping.on_entry_created = std::move(on_entry_created);
	mapping.on_entry_modified = std::move(on_entry_modified);
	mapping.on_entry_removed = std::move(on_entry_removed);
	mapping.on_entry_renamed = std::move(on_entry_renamed);
}

void syncer::unsync()
{
	const fs::path watch_dir = get_watch_path();
	fs::watcher::unwatch(watch_dir);
}

syncer::mapping syncer::get_mapping(const std::string& ext)
{
	std::lock_guard<std::mutex> lock(_mutex);
	auto it = _mapping.find(ext);
	if(it != _mapping.end())
	{
		return it->second;
	}

	return {};
}

syncer::on_entry_created_t syncer::get_on_created_callback(const std::string& ext)
{
	return get_mapping(ext).on_entry_created;
}

syncer::on_entry_modified_t syncer::get_on_modified_callback(const std::string& ext)
{
	return get_mapping(ext).on_entry_modified;
}

syncer::on_entry_removed_t syncer::get_on_removed_callback(const std::string& ext)
{
	return get_mapping(ext).on_entry_removed;
}

syncer::on_entry_renamed_t syncer::get_on_renamed_callback(const std::string& ext)
{
	return get_mapping(ext).on_entry_renamed;
}

void syncer::sync(const fs::path& reference_dir, const fs::path& synced_dir)
{
	unsync();

	{
		std::lock_guard<std::mutex> lock(_mutex);
		_reference_dir = reference_dir;
		_synced_dir = synced_dir;
		_reference_dir.make_preferred();
		_synced_dir.make_preferred();
		ensure_directory_exists(_reference_dir);
		ensure_directory_exists(_synced_dir);
	}

	const auto on_change = [this](const auto& entries, bool) {
		for(const auto& entry : entries)
		{
			bool is_directory = (entry.type == fs::directory_file);
			auto entry_extension = entry.path.extension().string();
			switch(entry.status)
			{
				case fs::watcher::entry_status::created:
				{
					const auto synced_entries = get_synced_entries(entry.path, is_directory);

					for(const auto& synced_entry : synced_entries)
					{
						ensure_directory_exists(synced_entry);
					}

					auto callback = get_on_created_callback(entry_extension);
					if(callback)
					{
						callback(entry.path, synced_entries);
					}
				}
				break;
				case fs::watcher::entry_status::modified:
				{
					auto callback = get_on_modified_callback(entry_extension);
					if(callback)
					{
						const auto synced_entries = get_synced_entries(entry.path, is_directory);
						callback(entry.path, synced_entries);
					}
				}
				break;
				case fs::watcher::entry_status::removed:
				{
					const auto callback = get_on_removed_callback(entry_extension);

					if(callback)
					{
						const auto synced_entries = get_synced_entries(entry.path, is_directory);
						callback(entry.path, synced_entries);
					}
				}
				break;
				case fs::watcher::entry_status::renamed:
				{
					const auto last_synced_entries = get_synced_entries(entry.last_path, is_directory);
					const auto synced_entries = get_synced_entries(entry.path, is_directory);
					auto callback = get_on_renamed_callback(entry_extension);

					if(callback)
					{
						assert(synced_entries.size() == last_synced_entries.size());

						std::vector<rename_pair_t> synced_renamed;
						synced_renamed.reserve(synced_entries.size());

						for(std::size_t i = 0; i < synced_entries.size(); ++i)
						{
							const auto& last_synced_entry = last_synced_entries[i];
							const auto& synced_entry = synced_entries[i];
							rename_pair_t p = {last_synced_entry, synced_entry};
							synced_renamed.emplace_back(std::move(p));
						}
						rename_pair_t p = {entry.last_path, entry.path};
						callback(p, synced_renamed);
					}
				}

				break;
				default:
					break;
			}
		}
	};

	const fs::path watch_dir = get_watch_path();
	fs::watcher::watch(watch_dir, true, true, on_change);
}

std::vector<fs::path> syncer::get_synced_entries(const fs::path& path, bool is_directory)
{
	std::vector<fs::path> synced_entries;
	auto synced_dir = get_synced_directory(path);

	if(is_directory)
	{
		synced_entries.emplace_back(std::move(synced_dir));
	}
	else
	{
		auto ext = path.extension().string();

		{
			std::lock_guard<std::mutex> lock(_mutex);
			auto it = _mapping.find(ext);
			if(it != _mapping.end())
			{
				const auto& mapping = it->second;
				const auto& extensions = mapping.extensions;

				synced_entries.reserve(extensions.size());
				for(const auto& cache_ext : extensions)
				{
					fs::path file = synced_dir / path.filename();
					file.concat(cache_ext);

					synced_entries.emplace_back(std::move(file));
				}
			}
		}
	}

	return synced_entries;
}

fs::path syncer::get_watch_path()
{
	std::lock_guard<std::mutex> lock(_mutex);
	const fs::path watch_dir = _reference_dir / "*";
	return watch_dir;
}

fs::path syncer::get_synced_directory(const fs::path& path)
{
	fs::path result;

	{
		std::lock_guard<std::mutex> lock(_mutex);
		result = fs::replace(path, _reference_dir, _synced_dir);
	}

	fs::error_code err;
	if(fs::is_directory(path, err) || !path.has_extension())
	{
		return result;
	}

	return result.parent_path();
}
}
