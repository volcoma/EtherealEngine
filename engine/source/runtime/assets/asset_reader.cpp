#include "asset_reader.h"
#include "../rendering/texture.h"
#include "../rendering/uniform.h"
#include "../rendering/shader.h"
#include "../rendering/mesh.h"
#include "../rendering/material.h"
#include "../rendering/vertex_buffer.h"
#include "../rendering/index_buffer.h"
#include "../system/filesystem.h"
#include "../ecs/prefab.h"
#include "../system/task.h"
#include "core/serialization/serialization.h"
#include "core/serialization/archives.h"
#include "core/serialization/cereal/types/unordered_map.hpp"
#include "core/serialization/cereal/types/vector.hpp"
#include "meta/rendering/material.hpp"
#include "meta/rendering/mesh.hpp"
#include <cstdint>

void AssetReader::load_texture_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Texture>& request)
{
	auto read_memory = std::make_shared<fs::byte_array_t>();

	auto readMemory = [read_memory, absoluteKey]()
	{
		if (!read_memory)
			return;

		*read_memory = fs::read_stream(std::ifstream{ absoluteKey, std::ios::in | std::ios::binary });
	};

	auto createResource = [read_memory, key, &request]() mutable
	{
		// if someone destroyed our memory
		if (!read_memory)
			return;
		// if nothing was read
		if (read_memory->empty())
			return;

		const gfx::Memory* mem = gfx::copy(read_memory->data(), static_cast<std::uint32_t>(read_memory->size()));
		read_memory->clear();
		read_memory.reset();

		if (nullptr != mem)
		{
			auto texture = std::make_shared<Texture>(mem, 0, 0, nullptr);
			request.set_data(key, texture);
			request.invoke_callbacks();
		}
		
	};

	if (async)
	{
		auto ts = core::get_subsystem<runtime::TaskSystem>();

		auto task = ts->create("", [ts, readMemory, createResource]()
		{
			readMemory();

			auto callback = ts->create("Create Resource", createResource);

			ts->run_on_main(callback);
		});
		request.set_task(task);
		ts->run(task);
	}
	else
	{
		readMemory();
		createResource();
	}

}

void AssetReader::load_shader_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Shader>& request)
{
	struct Wrapper
	{
		std::unordered_map<gfx::RendererType::Enum, fs::byte_array_t> binaries;
	};

	auto wrapper = std::make_shared<Wrapper>();
	auto deserialize = [wrapper, absoluteKey]() mutable
	{
		std::ifstream stream{ absoluteKey, std::ios::in | std::ios::binary };
		cereal::iarchive_binary_t ar(stream);

		try_load(ar, cereal::make_nvp("shader", wrapper->binaries));
	};

	auto createResource = [wrapper, key, &request]() mutable
	{
		auto& read_memory = wrapper->binaries[gfx::getRendererType()];
		const gfx::Memory* mem = gfx::copy(&read_memory[0], static_cast<std::uint32_t>(read_memory.size()));
		wrapper->binaries.clear();
		if (nullptr != mem)
		{
			auto shader = std::make_shared<Shader>();
			shader->populate(mem);
			auto uniform_count = gfx::getShaderUniforms(shader->handle);
			std::vector<gfx::UniformHandle> uniforms(uniform_count);
			gfx::getShaderUniforms(shader->handle, &uniforms[0], uniform_count);
			shader->uniforms.reserve(uniform_count);
			for (auto& uniform : uniforms)
			{
				std::shared_ptr<Uniform> hUniform = std::make_shared<Uniform>();
				hUniform->populate(uniform);

				shader->uniforms.push_back(hUniform);
			}

			request.set_data(key, shader);
			request.invoke_callbacks();
		}
		
	};

	if (async)
	{
		auto ts = core::get_subsystem<runtime::TaskSystem>();

		auto task = ts->create("", [ts, deserialize, createResource]() mutable
		{
			deserialize();

			auto callback = ts->create("Create Resource", createResource);

			ts->run_on_main(callback);
		});
		request.set_task(task);
		ts->run(task);
	}
	else
	{
		deserialize();
		createResource();
	}
}

void AssetReader::load_shader_from_memory(const std::string& key, const std::uint8_t* data, std::uint32_t size, LoadRequest<Shader>& request)
{
	auto createResource = [&key, data, size, &request]() mutable
	{
		// if nothing was read
		if (!data && size == 0)
			return;
		const gfx::Memory* mem = gfx::copy(data, size);
		if (nullptr != mem)
		{
			auto shader = std::make_shared<Shader>();
			shader->populate(mem);
			auto uniform_count = gfx::getShaderUniforms(shader->handle);
			std::vector<gfx::UniformHandle> uniforms(uniform_count);
			gfx::getShaderUniforms(shader->handle, &uniforms[0], uniform_count);
			shader->uniforms.reserve(uniform_count);
			for (auto& uniform : uniforms)
			{
				std::shared_ptr<Uniform> hUniform = std::make_shared<Uniform>();
				hUniform->populate(uniform);

				shader->uniforms.push_back(hUniform);
			}

			request.set_data(key, shader);
			request.invoke_callbacks();

		}
	};

	createResource();

}

void AssetReader::load_mesh_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Mesh>& request)
{
	struct Wrapper
	{
		std::shared_ptr<Mesh> mesh;
	};

	auto wrapper = std::make_shared<Wrapper>();
	auto deserialize = [wrapper, absoluteKey]() mutable
	{
		Mesh::LoadData data;
		{
			std::ifstream stream{ absoluteKey, std::ios::in | std::ios::binary };
			cereal::iarchive_binary_t ar(stream);

			try_load(ar, cereal::make_nvp("mesh", data));
		}
		
		wrapper->mesh = std::make_shared<Mesh>();
		wrapper->mesh->prepare_mesh(data.vertex_format, false);
		wrapper->mesh->set_vertex_source(&data.vertex_data[0], data.vertex_count, data.vertex_format);
		wrapper->mesh->add_primitives(data.triangle_data);
		wrapper->mesh->bind_skin(data.skin_data);
		wrapper->mesh->bind_armature(data.root_node);
		wrapper->mesh->end_prepare(true, false, false, false);
	};

	auto createResource = [wrapper, key, &request]() mutable
	{
		// Build the mesh
		wrapper->mesh->build_vb();
		wrapper->mesh->build_ib();

		if (wrapper->mesh->get_status() == MeshStatus::Prepared)
		{
			request.set_data(key, wrapper->mesh);
			request.invoke_callbacks();
		}
		wrapper.reset();
	};

	if (async)
	{
		auto ts = core::get_subsystem<runtime::TaskSystem>();

		auto task = ts->create("", [ts, deserialize, createResource]() mutable
		{
			deserialize();

			auto callback = ts->create("Create Resource", createResource);

			ts->run_on_main(callback);
		});
		request.set_task(task);
		ts->run(task);
	}
	else
	{
		deserialize();
		createResource();
	}
}

void AssetReader::load_material_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Material>& request)
{
	struct MatWrapper
	{
		std::shared_ptr<Material> hMaterial;
	};
	auto hMaterial = std::make_shared<Material>();
	auto matWrapper = std::make_shared<MatWrapper>();
	matWrapper->hMaterial = hMaterial;
	auto deserialize = [matWrapper, absoluteKey]() mutable
	{
		std::ifstream stream{ absoluteKey, std::ios::in | std::ios::binary };
		cereal::iarchive_json_t ar(stream);

		try_load(ar, cereal::make_nvp("material", matWrapper->hMaterial));
	};

	auto createResource = [matWrapper, key, absoluteKey, &request]() mutable
	{
		request.set_data(key, matWrapper->hMaterial);
		request.invoke_callbacks();
		matWrapper.reset();
	};

	if (async)
	{
		auto ts = core::get_subsystem<runtime::TaskSystem>();

		auto task = ts->create("", [ts, deserialize, createResource]() mutable
		{
			deserialize();

			auto callback = ts->create("Create Resource", createResource);

			ts->run_on_main(callback);
		});
		request.set_task(task);
		ts->run(task);
	}
	else
	{
		deserialize();
		createResource();
	}
}

void AssetReader::load_prefab_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Prefab>& request)
{

	std::shared_ptr<std::istringstream> read_memory = std::make_shared<std::istringstream>();

	auto readMemory = [read_memory, absoluteKey]()
	{
		if (!read_memory)
			return;

		auto mem = fs::read_stream(std::fstream{ absoluteKey, std::fstream::in | std::fstream::out | std::ios::binary });
 		*read_memory = std::istringstream(std::string(reinterpret_cast<const char*>(mem.data()), mem.size()));
	};

	auto createResource = [read_memory, key, absoluteKey, &request]() mutable
	{
		auto prefab = std::make_shared<Prefab>();
		prefab->data = read_memory;
		request.set_data(key, prefab);
		request.invoke_callbacks();
	};
	

	if (async)
	{
		auto ts = core::get_subsystem<runtime::TaskSystem>();

		auto task = ts->create("", [ts, readMemory, createResource]() mutable
		{
			readMemory();

			auto callback = ts->create("Create Resource", createResource);

			ts->run_on_main(callback);
		});
		request.set_task(task);
		ts->run(task);
	}
	else
	{
		readMemory();
		createResource();
	}
}

void AssetReader::load_scene_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Scene>& request)
{

	std::shared_ptr<std::istringstream> read_memory = std::make_shared<std::istringstream>();

	auto readMemory = [read_memory, absoluteKey]()
	{
		if (!read_memory)
			return;

		auto mem = fs::read_stream(std::fstream{ absoluteKey, std::fstream::in | std::fstream::out | std::ios::binary });
		*read_memory = std::istringstream(std::string(reinterpret_cast<const char*>(mem.data()), mem.size()));
	};

	auto createResource = [read_memory, key, absoluteKey, &request]() mutable
	{
		auto scene = std::make_shared<Scene>();
		scene->data = read_memory;
		request.set_data(key, scene);
		request.invoke_callbacks();
	};


	if (async)
	{
		auto ts = core::get_subsystem<runtime::TaskSystem>();

		auto task = ts->create("", [ts, readMemory, createResource]() mutable
		{
			readMemory();

			auto callback = ts->create("Create Resource", createResource);

			ts->run_on_main(callback);
		});
		request.set_task(task);
		ts->run(task);
	}
	else
	{
		readMemory();
		createResource();
	}
}