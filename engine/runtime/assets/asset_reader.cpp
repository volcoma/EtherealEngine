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

	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	auto compiled_absolute_key = absolute_key.string() + extensions::get_compiled_format<texture>();

	auto& ts = core::get_subsystem<core::task_system>();

	fs::error_code err;
	if(!fs::exists(compiled_absolute_key, err))
	{
		auto create_resource_func = [ result = original, key ]() mutable
		{
			result.link->id = key;
			return result;
		};
		output = ts.push_or_execute_on_worker_thread(create_resource_func);
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

	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	auto compiled_absolute_key = absolute_key.string() + extensions::get_compiled_format<shader>();

	auto& ts = core::get_subsystem<core::task_system>();

	fs::error_code err;
	if(!fs::exists(compiled_absolute_key, err))
	{
		auto create_resource_func = [ result = original, key ]() mutable
		{
			result.link->id = key;
			return result;
		};
		output = ts.push_or_execute_on_worker_thread(create_resource_func);
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
			auto shdr = std::make_shared<shader>();
			shdr->populate(mem);
			auto uniform_count = gfx::getShaderUniforms(shdr->handle);
			if(uniform_count > 0)
			{
				std::vector<gfx::UniformHandle> uniforms(uniform_count);
				gfx::getShaderUniforms(shdr->handle, &uniforms[0], uniform_count);
				shdr->uniforms.reserve(uniform_count);
				for(auto& uni : uniforms)
				{
					auto hUniform = std::make_shared<uniform>();
					hUniform->populate(uni);

					shdr->uniforms.push_back(hUniform);
				}
			}
			result.link->id = key;
			result.link->asset = shdr;
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
	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	auto compiled_absolute_key = absolute_key.string() + extensions::get_compiled_format<mesh>();

	auto& ts = core::get_subsystem<core::task_system>();

	fs::error_code err;
	if(!fs::exists(compiled_absolute_key, err))
	{
		auto create_resource_func = [ result = original, key ]() mutable
		{
			result.link->id = key;
			return result;
		};
		output = ts.push_or_execute_on_worker_thread(create_resource_func);
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
	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	auto compiled_absolute_key = absolute_key.string() + extensions::get_compiled_format<material>();

	auto& ts = core::get_subsystem<core::task_system>();

	fs::error_code err;
	if(!fs::exists(compiled_absolute_key, err))
	{
		auto create_resource_func = [ result = original, key ]() mutable
		{
			result.link->id = key;
			return result;
		};
		output = ts.push_or_execute_on_worker_thread(create_resource_func);
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

	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	auto compiled_absolute_key = absolute_key.string() + extensions::get_compiled_format<prefab>();

	auto& ts = core::get_subsystem<core::task_system>();

	fs::error_code err;
	if(!fs::exists(compiled_absolute_key, err))
	{
		auto create_resource_func = [ result = original, key ]() mutable
		{
			result.link->id = key;
			return result;
		};
		output = ts.push_or_execute_on_worker_thread(create_resource_func);
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

	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	auto compiled_absolute_key = absolute_key.string() + extensions::get_compiled_format<scene>();

	auto& ts = core::get_subsystem<core::task_system>();

	fs::error_code err;
	if(!fs::exists(compiled_absolute_key, err))
	{
		auto create_resource_func = [ result = original, key ]() mutable
		{
			result.link->id = key;
			return result;
		};
		output = ts.push_or_execute_on_worker_thread(create_resource_func);
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

template <>
bool load_from_memory<shader>(core::task_future<asset_handle<shader>>& output, const std::string& key,
							  const std::uint8_t* data, std::uint32_t size)
{
	asset_handle<shader> original;
	if(output.is_ready())
		original = output.get();

	auto create_resource_func = [&key, data, size, result = original ]() mutable
	{
		result.link->id = key;
		// if nothing was read
		if(!data && size == 0)
			return result;
		const gfx::Memory* mem = gfx::copy(data, size);
		if(nullptr != mem)
		{
			auto shdr = std::make_shared<shader>();
			shdr->populate(mem);
			auto uniform_count = gfx::getShaderUniforms(shdr->handle);
			if(uniform_count > 0)
			{
				std::vector<gfx::UniformHandle> uniforms(uniform_count);
				gfx::getShaderUniforms(shdr->handle, &uniforms[0], uniform_count);
				shdr->uniforms.reserve(uniform_count);
				for(auto& uni : uniforms)
				{
					auto hUniform = std::make_shared<uniform>();
					hUniform->populate(uni);

					shdr->uniforms.push_back(hUniform);
				}
			}

			result.link->asset = shdr;
		}
		return result;
	};

	auto& ts = core::get_subsystem<core::task_system>();
	output = ts.push_or_execute_on_owner_thread(create_resource_func);
	return true;
}
}
}
