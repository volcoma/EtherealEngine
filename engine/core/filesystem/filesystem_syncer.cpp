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
	std::lock_guard<std::mutex> lock(mutex_);
	auto& mapping = mapping_[ref_ext];
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
	std::lock_guard<std::mutex> lock(mutex_);
	auto& mapping = mapping_[""];
	mapping.on_entry_created = std::move(on_entry_created);
	mapping.on_entry_modified = std::move(on_entry_modified);
	mapping.on_entry_removed = std::move(on_entry_removed);
	mapping.on_entry_renamed = std::move(on_entry_renamed);
}

void syncer::unsync()
{
	fs::watcher::unwatch(watch_id_);
}

syncer::mapping syncer::get_mapping(const std::string& ext)
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = mapping_.find(ext);
	if(it != mapping_.end())
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
		std::lock_guard<std::mutex> lock(mutex_);
		reference_dir_ = reference_dir;
		synced_dir_ = synced_dir;
		reference_dir_.make_preferred();
		synced_dir_.make_preferred();
		ensure_directory_exists(reference_dir_);
		ensure_directory_exists(synced_dir_);
	}

	const auto on_change = [this](const auto& entries, bool is_initial_listing) {
		for(const auto& entry : entries)
		{
			bool is_directory = (entry.type == fs::directory_file);
			auto entry_path = entry.path;
			std::string entry_extension;
			while(entry_path.has_extension())
			{
				entry_extension = entry_path.extension().string() + entry_extension;
				entry_path.replace_extension();
			}
			switch(entry.status)
			{
				case fs::watcher::entry_status::created:
				{
					const auto synced_entries = this->get_synced_entries(entry.path, is_directory);

					for(const auto& synced_entry : synced_entries)
					{
						ensure_directory_exists(synced_entry);
					}

					auto callback = this->get_on_created_callback(entry_extension);
					if(callback)
					{
						callback(entry.path, synced_entries, is_initial_listing);
					}
				}
				break;
				case fs::watcher::entry_status::modified:
				{
					auto callback = this->get_on_modified_callback(entry_extension);
					if(callback)
					{
						const auto synced_entries = this->get_synced_entries(entry.path, is_directory);
						callback(entry.path, synced_entries, is_initial_listing);
					}
				}
				break;
				case fs::watcher::entry_status::removed:
				{
					const auto callback = this->get_on_removed_callback(entry_extension);

					if(callback)
					{
						const auto synced_entries = this->get_synced_entries(entry.path, is_directory);
						callback(entry.path, synced_entries);
					}
				}
				break;
				case fs::watcher::entry_status::renamed:
				{
					const auto last_synced_entries = this->get_synced_entries(entry.last_path, is_directory);
					const auto synced_entries = this->get_synced_entries(entry.path, is_directory);
					auto callback = this->get_on_renamed_callback(entry_extension);

					if(callback && synced_entries.size() == last_synced_entries.size())
					{
						std::vector<rename_pair_t> synced_renamed;
						synced_renamed.reserve(synced_entries.size());

						for(std::size_t i = 0; i < synced_entries.size(); ++i)
						{
							const auto& last_synced_entry = last_synced_entries[i];
							const auto& synced_entry = synced_entries[i];
							rename_pair_t p(last_synced_entry, synced_entry);
							synced_renamed.emplace_back(std::move(p));
						}
						rename_pair_t p(entry.last_path, entry.path);
						callback(p, synced_renamed);
					}
				}

				break;
				default:
					break;
			}
		}
	};
	using namespace std::literals;
	const fs::path watch_dir = get_watch_path();
	watch_id_ = fs::watcher::watch(watch_dir, true, true, 500ms, on_change);
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
		auto entry_path = path;
		std::string entry_extension;
		while(entry_path.has_extension())
		{
			auto ext = entry_path.extension().string() + entry_extension;
			entry_extension = ext;
			entry_path.replace_extension();
		}

		{
			std::lock_guard<std::mutex> lock(mutex_);
			auto it = mapping_.find(entry_extension);
			if(it != mapping_.end())
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
	std::lock_guard<std::mutex> lock(mutex_);
	const fs::path watch_dir = reference_dir_ / "*";
	return watch_dir;
}

fs::path syncer::get_synced_directory(const fs::path& path)
{
	fs::path result;

	{
		std::lock_guard<std::mutex> lock(mutex_);
		result = fs::replace(path, reference_dir_, synced_dir_);
	}

	fs::error_code err;
	if(fs::is_directory(path, err) || !path.has_extension())
	{
		return result;
	}

	return result.parent_path();
}
}
