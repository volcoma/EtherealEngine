#pragma once
#include "asset_handle.h"
#include "core/filesystem/filesystem.h"
#include "core/system/task_system.h"

namespace runtime
{
namespace asset_reader
{
template <typename T>
extern bool load_from_file(core::task_future<asset_handle<T>>& output, const std::string& key);

template <typename T>
inline bool load_from_instance(core::task_future<asset_handle<T>>& output, const std::string& key,
															 std::shared_ptr<T> instance)
{
	auto& ts = core::get_subsystem<core::task_system>();
	output = ts.push_or_execute_on_owner_thread(
		[](const std::string& key, std::shared_ptr<T> instance) {
			asset_handle<T> handle;
			handle.link->id = key;
			handle.link->asset = instance;

			return handle;
		},
		key, instance);

	return true;
}
}
}
