#pragma once
#include "asset_handle.h"
#include "core/filesystem/filesystem.h"
#include "core/system/task_system.h"

namespace runtime
{
namespace asset_reader
{
template <typename T>
extern core::task_future<asset_handle<T>> load_from_file(const std::string& key, asset_handle<T> original);

template <typename T>
extern core::task_future<asset_handle<T>> load_from_memory(const std::string& key, const std::uint8_t* data,
														   std::uint32_t size);

template <typename T>
inline core::task_future<asset_handle<T>> load_from_instance(const std::string& key,
															 std::shared_ptr<T> instance)
{
	auto& ts = core::get_subsystem<core::task_system>();
	auto create_handle = ts.push_ready_on_main(
		[](const std::string& key, std::shared_ptr<T> instance) {
			asset_handle<T> handle;
			handle.link->id = key;
			handle.link->asset = instance;

			return handle;
		},
		key, instance);

	return create_handle;
}
};
}
