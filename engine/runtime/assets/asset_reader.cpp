#include "asset_reader.h"
#include "../rendering/texture.h"
#include "../rendering/uniform.h"
#include "../rendering/shader.h"
#include "../rendering/mesh.h"
#include "../rendering/material.h"
#include "../rendering/vertex_buffer.h"
#include "../rendering/index_buffer.h"
#include "../ecs/prefab.h"
#include "../system/task.h"
#include "core/filesystem/filesystem.h"
#include "core/serialization/serialization.h"
#include "core/serialization/binary_archive.h"
#include "core/serialization/associative_archive.h"
#include "core/serialization/types/map.hpp"
#include "core/serialization/types/vector.hpp"
#include "../meta/rendering/material.hpp"
#include "../meta/rendering/mesh.hpp"
#include <cstdint>

namespace runtime
{
	void asset_reader::load_texture_from_file(const std::string& key, const fs::path& absolute_key, bool async, request<texture>& request)
	{
		auto read_memory = std::make_shared<fs::byte_array_t>();
		auto compiled_absolute_key = absolute_key.string() + extensions::compiled;

		auto read_memory_func = [read_memory, compiled_absolute_key]()
		{
			if (!read_memory)
				return;

			auto stream = std::ifstream{ compiled_absolute_key, std::ios::in | std::ios::binary };
			*read_memory = fs::read_stream(stream);
		};

		auto create_resource_func = [read_memory, key, &request]() mutable
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
				auto tex = std::make_shared<texture>(mem, 0, 0, nullptr);
				request.set_data(key, tex);
				request.invoke_callbacks();
			}

		};

		if (async)
		{
			auto& ts = core::get_subsystem<runtime::task_system>();

			auto task = ts.create("", [&ts, read_memory_func, create_resource_func]()
			{
				read_memory_func();

				auto callback = ts.create("Create Resource", create_resource_func);

				ts.run(callback, true);
			});
			request.set_task(task);
			ts.run(task);
		}
		else
		{
			read_memory_func();
			create_resource_func();
		}

	}

	void asset_reader::load_shader_from_file(const std::string& key, const fs::path& absolute_key, bool async, request<shader>& request)
	{
		struct Wrapper
		{
			std::map<gfx::RendererType::Enum, fs::byte_array_t> binaries;
		};

		auto compiled_absolute_key = absolute_key.string() + extensions::compiled;

		auto wrapper = std::make_shared<Wrapper>();
		auto deserialize = [wrapper, compiled_absolute_key]() mutable
		{
			std::ifstream stream{ compiled_absolute_key, std::ios::in | std::ios::binary };
			cereal::iarchive_binary_t ar(stream);

			try_load(ar, cereal::make_nvp("shader", wrapper->binaries));
		};

		auto create_resource_func = [wrapper, key, &request]() mutable
		{
			auto& read_memory = wrapper->binaries[gfx::getRendererType()];
			const gfx::Memory* mem = gfx::copy(&read_memory[0], static_cast<std::uint32_t>(read_memory.size()));
			wrapper->binaries.clear();
			if (nullptr != mem)
			{
				auto shdr = std::make_shared<shader>();
				shdr->populate(mem);
				auto uniform_count = gfx::getShaderUniforms(shdr->handle);
				if (uniform_count > 0)
				{
					std::vector<gfx::UniformHandle> uniforms(uniform_count);
					gfx::getShaderUniforms(shdr->handle, &uniforms[0], uniform_count);
					shdr->uniforms.reserve(uniform_count);
					for (auto& uni : uniforms)
					{
						auto hUniform = std::make_shared<uniform>();
						hUniform->populate(uni);

						shdr->uniforms.push_back(hUniform);
					}
				}

				request.set_data(key, shdr);
				request.invoke_callbacks();
			}

		};

		if (async)
		{
			auto& ts = core::get_subsystem<runtime::task_system>();

			auto task = ts.create("", [&ts, deserialize, create_resource_func]() mutable
			{
				deserialize();

				auto callback = ts.create("Create Resource", create_resource_func);

				ts.run(callback, true);
			});
			request.set_task(task);
			ts.run(task);
		}
		else
		{
			deserialize();
			create_resource_func();
		}
	}

	void asset_reader::load_shader_from_memory(const std::string& key, const std::uint8_t* data, std::uint32_t size, request<shader>& request)
	{
		auto create_resource_func = [&key, data, size, &request]() mutable
		{
			// if nothing was read
			if (!data && size == 0)
				return;
			const gfx::Memory* mem = gfx::copy(data, size);
			if (nullptr != mem)
			{
				auto shdr = std::make_shared<shader>();
				shdr->populate(mem);
				auto uniform_count = gfx::getShaderUniforms(shdr->handle);
				if (uniform_count > 0)
				{
					std::vector<gfx::UniformHandle> uniforms(uniform_count);
					gfx::getShaderUniforms(shdr->handle, &uniforms[0], uniform_count);
					shdr->uniforms.reserve(uniform_count);
					for (auto& uni : uniforms)
					{
						auto hUniform = std::make_shared<uniform>();
						hUniform->populate(uni);

						shdr->uniforms.push_back(hUniform);
					}
				}


				request.set_data(key, shdr);
				request.invoke_callbacks();

			}
		};

		create_resource_func();

	}

	void asset_reader::load_mesh_from_file(const std::string& key, const fs::path& absolute_key, bool async, request<mesh>& request)
	{
        struct wrapper_t
		{
            std::shared_ptr<::mesh> mesh;
		};

		auto compiled_absolute_key = absolute_key.string() + extensions::compiled;

        auto wrapper = std::make_shared<wrapper_t>();
		wrapper->mesh = std::make_shared<mesh>();
		auto deserialize = [wrapper, compiled_absolute_key]() mutable
		{
			mesh::load_data data;
			{
				std::ifstream stream{ compiled_absolute_key, std::ios::in | std::ios::binary };
				cereal::iarchive_binary_t ar(stream);

				try_load(ar, cereal::make_nvp("mesh", data));
			}
			wrapper->mesh->prepare_mesh(data.vertex_format);
			wrapper->mesh->set_vertex_source(&data.vertex_data[0], data.vertex_count, data.vertex_format);
			wrapper->mesh->add_primitives(data.triangle_data);
			wrapper->mesh->bind_skin(data.skin_data);
			wrapper->mesh->bind_armature(data.root_node);
			wrapper->mesh->end_prepare(true, false, false, false);
		};

		auto create_resource_func = [wrapper, key, &request]() mutable
		{
			// Build the mesh
			wrapper->mesh->build_vb();
			wrapper->mesh->build_ib();

			if (wrapper->mesh->get_status() == mesh_status::prepared)
			{
				request.set_data(key, wrapper->mesh);
				request.invoke_callbacks();
			}
			wrapper.reset();
		};

		if (async)
		{
			auto& ts = core::get_subsystem<runtime::task_system>();

			auto task = ts.create("", [&ts, deserialize, create_resource_func]() mutable
			{
				deserialize();

				auto callback = ts.create("Create Resource", create_resource_func);

				ts.run(callback, true);
			});
			request.set_task(task);
			ts.run(task);
		}
		else
		{
			deserialize();
			create_resource_func();
		}
	}

	void asset_reader::load_material_from_file(const std::string& key, const fs::path& absolute_key, bool async, request<material>& request)
	{
        struct wrapper_t
		{
            std::shared_ptr<::material> material;
		};

        auto wrapper = std::make_shared<wrapper_t>();
		wrapper->material = std::make_shared<material>();
		auto deserialize = [wrapper, absolute_key]() mutable
		{
			std::ifstream stream{ absolute_key.string(), std::ios::in | std::ios::binary };
			cereal::iarchive_associative_t ar(stream);

			try_load(ar, cereal::make_nvp("material", wrapper->material));
		};

		auto create_resource_func = [wrapper, key, absolute_key, &request]() mutable
		{
			request.set_data(key, wrapper->material);
			request.invoke_callbacks();
			wrapper.reset();
		};

		if (async)
		{
			auto& ts = core::get_subsystem<runtime::task_system>();

			auto task = ts.create("", [&ts, deserialize, create_resource_func]() mutable
			{
				deserialize();

				auto callback = ts.create("Create Resource", create_resource_func);

				ts.run(callback, true);
			});
			request.set_task(task);
			ts.run(task);
		}
		else
		{
			deserialize();
			create_resource_func();
		}
	}

	void asset_reader::load_prefab_from_file(const std::string& key, const fs::path& absolute_key, bool async, request<prefab>& request)
	{

		std::shared_ptr<std::istringstream> read_memory = std::make_shared<std::istringstream>();

		auto read_memory_func = [read_memory, absolute_key]()
		{
			if (!read_memory)
				return;

			auto stream = std::fstream{ absolute_key.string(), std::fstream::in | std::fstream::out | std::ios::binary };
			auto mem = fs::read_stream(stream);
			*read_memory = std::istringstream(std::string(reinterpret_cast<const char*>(mem.data()), mem.size()));
		};

		auto create_resource_func = [read_memory, key, absolute_key, &request]() mutable
		{
			auto pfab = std::make_shared<prefab>();
			pfab->data = read_memory;
			request.set_data(key, pfab);
			request.invoke_callbacks();
		};


		if (async)
		{
			auto& ts = core::get_subsystem<runtime::task_system>();

			auto task = ts.create("", [&ts, read_memory_func, create_resource_func]() mutable
			{
				read_memory_func();

				auto callback = ts.create("Create Resource", create_resource_func);

				ts.run(callback, true);
			});
			request.set_task(task);
			ts.run(task);
		}
		else
		{
			read_memory_func();
			create_resource_func();
		}
	}

	void asset_reader::load_scene_from_file(const std::string& key, const fs::path& absolute_key, bool async, request<scene>& request)
	{

		std::shared_ptr<std::istringstream> read_memory = std::make_shared<std::istringstream>();

		auto read_memory_func = [read_memory, absolute_key]()
		{
			if (!read_memory)
				return;

			auto stream = std::fstream{ absolute_key.string(), std::fstream::in | std::fstream::out | std::ios::binary };
			auto mem = fs::read_stream(stream);
			*read_memory = std::istringstream(std::string(reinterpret_cast<const char*>(mem.data()), mem.size()));
		};

		auto create_resource_func = [read_memory, key, absolute_key, &request]() mutable
		{
			auto sc = std::make_shared<scene>();
			sc->data = read_memory;
			request.set_data(key, sc);
			request.invoke_callbacks();
		};


		if (async)
		{
			auto& ts = core::get_subsystem<runtime::task_system>();

			auto task = ts.create("", [&ts, read_memory_func, create_resource_func]() mutable
			{
				read_memory_func();

				auto callback = ts.create("Create Resource", create_resource_func);

				ts.run(callback, true);
			});
			request.set_task(task);
			ts.run(task);
		}
		else
		{
			read_memory_func();
			create_resource_func();
		}
	}
}
