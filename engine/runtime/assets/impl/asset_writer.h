#pragma once
#include "../asset_handle.h"
#include "core/filesystem/filesystem.h"
#include <string>

class material;
namespace runtime
{
namespace asset_writer
{
template <typename T>
extern void save_to_file(const fs::path& key, const asset_handle<T>& asset);

template <typename T>
inline void rename_asset_file(const std::string& key, const std::string& new_key)
{
	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	fs::path absolute_new_key = fs::absolute(fs::resolve_protocol(new_key).string());
	fs::error_code err;
	fs::rename(absolute_key, absolute_new_key, err);
}

template <typename T>
inline void delete_asset_file(const std::string& key)
{
	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string()).make_preferred();
	fs::error_code err;
	fs::remove(absolute_key, err);
}
}
}
