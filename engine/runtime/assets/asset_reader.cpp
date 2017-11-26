#include "asset_reader.h"
#include "../ecs/prefab.h"
#include "../ecs/scene.h"
#include "../meta/rendering/material.hpp"
#include "../meta/rendering/mesh.hpp"
#include "../rendering/index_buffer.h"
#include "../rendering/material.h"
#include "../rendering/mesh.h"
#include "../rendering/shader.h"
#include "../rendering/texture.h"
#include "../rendering/uniform.h"
#include "../rendering/vertex_buffer.h"
#include "asset_extensions.h"
#include "core/filesystem/filesystem.h"
#include "core/logging/logging.h"
#include "core/serialization/associative_archive.h"
#include "core/serialization/binary_archive.h"
#include "core/serialization/serialization.h"
#include "core/serialization/types/map.hpp"
#include "core/serialization/types/vector.hpp"
#include <cstdint>

namespace runtime
{
namespace asset_reader
{
template <>
bool load_from_file<texture>(core::task_future<asset_handle<texture>>& output, const std::string& key)
{
	asset_handle<texture> original;
	if(output.is_ready())
		original = output.get();

	auto& ts = core::get_subsystem<core::task_system>();

	auto create_resource_func_fallback = [result = original, key]() mutable
	{
		result.link->id = key;
		return result;
	};

	if (!fs::has_known_protocol(key))
	{
		APPLOG_ERROR("Asset {0} has uknown protocol!", key);
		output = ts.push_or_execute_on_worker_thread(create_resource_func_fallback);
		return true;
	}

	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	auto compiled_absolute_key = absolute_key.string() + extensions::get_compiled_format<texture>();

	fs::error_code err;
	if(!fs::exists(compiled_absolute_key, err))
	{
		APPLOG_ERROR("Asset {0} does not exist!", key);
		output = ts.push_or_execute_on_worker_thread(create_resource_func_fallback);
		return true;
	}

	auto read_memory = std::make_shared<fs::byte_array_t>();
	auto read_memory_func = [read_memory, compiled_absolute_key]() {
		if(!read_memory)
			return false;

		auto stream = std::ifstream{compiled_absolute_key, std::ios::in | std::ios::binary};
		*read_memory = fs::read_stream(stream);

		return true;
	};

	auto create_resource_func = [ result = original, read_memory, key ](bool read_result) mutable
	{
		// if someone destroyed our memory
		if(!read_memory)
			return result;
		// if nothing was read
		if(read_memory->empty())
			return result;

		const gfx::Memory* mem =
			gfx::copy(read_memory->data(), static_cast<std::uint32_t>(read_memory->size()));

		read_memory->clear();
		read_memory.reset();

		if(nullptr != mem)
		{
			auto tex = std::make_shared<texture>(mem, 0, 0, nullptr);
			result.link->id = key;
			result.link->asset = tex;
		}

		return result;
	};

	auto ready_memory_task = ts.push_on_worker_thread(read_memory_func);
	output = ts.push_on_owner_thread(create_resource_func, ready_memory_task);
	return true;
}

template <>
bool load_from_file<shader>(core::task_future<asset_handle<shader>>& output, const std::string& key)
{
	asset_handle<shader> original;
	if(output.is_ready())
		original = output.get();

	auto& ts = core::get_subsystem<core::task_system>();

	auto create_resource_func_fallback = [result = original, key]() mutable
	{
		result.link->id = key;
		return result;
	};

	if (!fs::has_known_protocol(key))
	{
		APPLOG_ERROR("Asset {0} has uknown protocol!", key);
		output = ts.push_or_execute_on_worker_thread(create_resource_func_fallback);
		return true;
	}

	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	auto compiled_absolute_key = absolute_key.string() + extensions::get_compiled_format<shader>();

	fs::error_code err;
	if(!fs::exists(compiled_absolute_key, err))
	{
		APPLOG_ERROR("Asset {0} does not exist!", key);
		output = ts.push_or_execute_on_worker_thread(create_resource_func_fallback);
		return true;
	}

	auto read_memory = std::make_shared<fs::byte_array_t>();

	auto read_memory_func = [read_memory, compiled_absolute_key]() {
		if(!read_memory)
			return false;

		auto stream = std::ifstream{compiled_absolute_key, std::ios::in | std::ios::binary};
		*read_memory = fs::read_stream(stream);

		return true;
	};

	auto create_resource_func = [ result = original, read_memory, key ](bool read_result) mutable
	{
		// if someone destroyed our memory
		if(!read_memory)
			return result;
		// if nothing was read
		if(read_memory->empty())
			return result;

		const gfx::Memory* mem =
			gfx::copy(read_memory->data(), static_cast<std::uint32_t>(read_memory->size()));
		read_memory->clear();
		read_memory.reset();

		if(nullptr != mem)
		{
			result.link->id = key;
			result.link->asset = std::make_shared<shader>(mem);
		}

		return result;
	};

	auto ready_memory_task = ts.push_on_worker_thread(read_memory_func);
	output = ts.push_on_owner_thread(create_resource_func, ready_memory_task);
	return true;
}

template <>
bool load_from_file<mesh>(core::task_future<asset_handle<mesh>>& output, const std::string& key)
{
	asset_handle<mesh> original;
	if(output.is_ready())
		original = output.get();

	auto& ts = core::get_subsystem<core::task_system>();

	auto create_resource_func_fallback = [result = original, key]() mutable
	{
		result.link->id = key;
		return result;
	};

	if (!fs::has_known_protocol(key))
	{
		APPLOG_ERROR("Asset {0} has uknown protocol!", key);
		output = ts.push_or_execute_on_worker_thread(create_resource_func_fallback);
		return true;
	}

	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	auto compiled_absolute_key = absolute_key.string() + extensions::get_compiled_format<mesh>();

	fs::error_code err;
	if(!fs::exists(compiled_absolute_key, err))
	{
		APPLOG_ERROR("Asset {0} does not exist!", key);
		output = ts.push_or_execute_on_worker_thread(create_resource_func_fallback);
		return true;
	}

	struct wrapper_t
	{
		std::shared_ptr<::mesh> mesh;
	};

	auto wrapper = std::make_shared<wrapper_t>();
	wrapper->mesh = std::make_shared<mesh>();
	auto read_memory_func = [wrapper, compiled_absolute_key]() mutable {
		mesh::load_data data;
		{
			std::ifstream stream{compiled_absolute_key, std::ios::in | std::ios::binary};

			if(stream.bad())
			{
				return false;
			}

			cereal::iarchive_binary_t ar(stream);

			try_load(ar, cereal::make_nvp("mesh", data));
		}
		wrapper->mesh->prepare_mesh(data.vertex_format);
		wrapper->mesh->set_vertex_source(&data.vertex_data[0], data.vertex_count, data.vertex_format);
		wrapper->mesh->add_primitives(data.triangle_data);
		wrapper->mesh->set_subset_count(data.material_count);
		wrapper->mesh->bind_skin(data.skin_data);
		wrapper->mesh->bind_armature(data.root_node);
		wrapper->mesh->end_prepare(true, false, false, false);

		return true;
	};

	auto create_resource_func = [ result = original, wrapper, key ](bool read_result) mutable
	{
		// Build the mesh
		if(read_result)
		{
			wrapper->mesh->build_vb();
			wrapper->mesh->build_ib();

			if(wrapper->mesh->get_status() == mesh_status::prepared)
			{
				result.link->id = key;
				result.link->asset = wrapper->mesh;
			}
			wrapper.reset();
		}

		return result;
	};

	auto ready_memory_task = ts.push_on_worker_thread(read_memory_func);
	output = ts.push_on_owner_thread(create_resource_func, ready_memory_task);
	return true;
}

template <>
bool load_from_file<material>(core::task_future<asset_handle<material>>& output, const std::string& key)
{
	asset_handle<material> original;
	if(output.is_ready())
		original = output.get();

	auto& ts = core::get_subsystem<core::task_system>();

	auto create_resource_func_fallback = [result = original, key]() mutable
	{
		result.link->id = key;
		return result;
	};

	if (!fs::has_known_protocol(key))
	{
		APPLOG_ERROR("Asset {0} has uknown protocol!", key);
		output = ts.push_or_execute_on_worker_thread(create_resource_func_fallback);
		return true;
	}

	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	auto compiled_absolute_key = absolute_key.string() + extensions::get_compiled_format<material>();


	fs::error_code err;
	if(!fs::exists(compiled_absolute_key, err))
	{
		APPLOG_ERROR("Asset {0} does not exist!", key);
		output = ts.push_or_execute_on_worker_thread(create_resource_func_fallback);
		return true;
	}

	struct wrapper_t
	{
		std::shared_ptr<::material> material;
	};

	auto wrapper = std::make_shared<wrapper_t>();
	wrapper->material = std::make_shared<material>();

	auto read_memory_func = [wrapper, compiled_absolute_key]() mutable {
		std::ifstream stream{compiled_absolute_key, std::ios::in | std::ios::binary};

		if(stream.bad())
		{
			return false;
		}
		cereal::iarchive_associative_t ar(stream);

		try_load(ar, cereal::make_nvp("material", wrapper->material));

		return true;
	};

	auto create_resource_func = [ result = original, wrapper, key ](bool read_result) mutable
	{
		result.link->id = key;
		result.link->asset = wrapper->material;
		wrapper.reset();

		return result;
	};

	auto ready_memory_task = ts.push_on_worker_thread(read_memory_func);
	output = ts.push_on_owner_thread(create_resource_func, ready_memory_task);
	return true;
}

template <>
bool load_from_file<prefab>(core::task_future<asset_handle<prefab>>& output, const std::string& key)
{
	asset_handle<prefab> original;
	if(output.is_ready())
		original = output.get();

	auto& ts = core::get_subsystem<core::task_system>();

	auto create_resource_func_fallback = [result = original, key]() mutable
	{
		result.link->id = key;
		return result;
	};

	if (!fs::has_known_protocol(key))
	{
		APPLOG_ERROR("Asset {0} has uknown protocol!", key);
		output = ts.push_or_execute_on_worker_thread(create_resource_func_fallback);
		return true;
	}

	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	auto compiled_absolute_key = absolute_key.string() + extensions::get_compiled_format<prefab>();

	fs::error_code err;
	if(!fs::exists(compiled_absolute_key, err))
	{
		APPLOG_ERROR("Asset {0} does not exist!", key);
		output = ts.push_or_execute_on_worker_thread(create_resource_func_fallback);
		return true;
	}

	std::shared_ptr<std::istringstream> read_memory = std::make_shared<std::istringstream>();

	auto read_memory_func = [read_memory, compiled_absolute_key]() {
		if(!read_memory)
			return false;

		auto stream =
			std::fstream{compiled_absolute_key, std::fstream::in | std::fstream::out | std::ios::binary};
		auto mem = fs::read_stream(stream);
		*read_memory = std::istringstream(std::string(mem.begin(), mem.end()));

		return true;
	};

	auto create_resource_func = [ result = original, read_memory, key ](bool read_result) mutable
	{
		if(read_result)
		{
			auto pfab = std::make_shared<prefab>();
			pfab->data = read_memory;

			result.link->id = key;
			result.link->asset = pfab;
		}

		return result;
	};

	auto ready_memory_task = ts.push_on_worker_thread(read_memory_func);
	output = ts.push_on_owner_thread(create_resource_func, ready_memory_task);
	return true;
}

template <>
bool load_from_file<scene>(core::task_future<asset_handle<scene>>& output, const std::string& key)
{
	asset_handle<scene> original;
	if(output.is_ready())
		original = output.get();

	auto& ts = core::get_subsystem<core::task_system>();

	auto create_resource_func_fallback = [result = original, key]() mutable
	{
		result.link->id = key;
		return result;
	};

	if (!fs::has_known_protocol(key))
	{
		APPLOG_ERROR("Asset {0} has uknown protocol!", key);
		output = ts.push_or_execute_on_worker_thread(create_resource_func_fallback);
		return true;
	}

	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	auto compiled_absolute_key = absolute_key.string() + extensions::get_compiled_format<scene>();

	fs::error_code err;
	if(!fs::exists(compiled_absolute_key, err))
	{
		APPLOG_ERROR("Asset {0} does not exist!", key);
		output = ts.push_or_execute_on_worker_thread(create_resource_func_fallback);
		return true;
	}

	std::shared_ptr<std::istringstream> read_memory = std::make_shared<std::istringstream>();

	auto read_memory_func = [read_memory, compiled_absolute_key]() {
		if(!read_memory)
			return false;

		auto stream =
			std::fstream{compiled_absolute_key, std::fstream::in | std::fstream::out | std::ios::binary};
		auto mem = fs::read_stream(stream);
		*read_memory = std::istringstream(std::string(mem.begin(), mem.end()));

		return true;
	};

	auto create_resource_func = [ result = original, read_memory, key ](bool read_result) mutable
	{
		if(read_result)
		{
			auto sc = std::make_shared<scene>();
			sc->data = read_memory;

			result.link->id = key;
			result.link->asset = sc;
		}

		return result;
	};

	auto ready_memory_task = ts.push_on_worker_thread(read_memory_func);
	output = ts.push_on_owner_thread(create_resource_func, ready_memory_task);
	return true;
}
}
}
