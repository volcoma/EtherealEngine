#pragma once
#include <string>
#include "asset_handle.h"
#include "core/filesystem/filesystem.h"

class material;
namespace runtime
{
	struct asset_writer
	{
		template<typename T>
		static void save_to_file(const fs::path& key, const asset_handle<T>& asset);

		template<typename T>
		static void rename_asset_file(const std::string& key, const std::string& new_key)
		{
			fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
			fs::path absolute_new_key = fs::absolute(fs::resolve_protocol(new_key).string());
			fs::error_code err;
			fs::rename(absolute_key, absolute_new_key, err);
		}

		template<typename T>
		static void delete_asset_file(const std::string& key)
		{
			fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
			fs::error_code err;
			fs::remove(absolute_key, err);
		}
	};
}
