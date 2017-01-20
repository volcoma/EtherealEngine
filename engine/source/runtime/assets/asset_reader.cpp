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
#include <cstdint>

#include "ib-compress/indexbufferdecompression.h"

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
		cereal::iarchive_json_t ar(stream);

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

struct MeshData
{
	gfx::VertexDecl decl;
	std::vector<Group> groups;
	MeshInfo info;
	std::vector<std::pair<fs::byte_array_t, fs::byte_array_t>> buffersMem; // vb, ib
	math::bbox aabb;
};


void AssetReader::load_mesh_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Mesh>& request)
{
#define BGFX_CHUNK_MAGIC_VB  BX_MAKEFOURCC('V', 'B', ' ', 0x1)
#define BGFX_CHUNK_MAGIC_IB  BX_MAKEFOURCC('I', 'B', ' ', 0x0)
#define BGFX_CHUNK_MAGIC_IBC BX_MAKEFOURCC('I', 'B', 'C', 0x0)
#define BGFX_CHUNK_MAGIC_PRI BX_MAKEFOURCC('P', 'R', 'I', 0x0)

	std::shared_ptr<MeshData> data = std::make_shared<MeshData>();

	auto readMemory = [data, absoluteKey]()
	{
		if (!data)
			return;

		FileStreamReaderSeeker _reader(absoluteKey.string());

		std::pair<fs::byte_array_t, fs::byte_array_t> buffers;
		std::uint16_t numVertices = 0;
		std::uint32_t numIndices = 0;
		std::uint32_t chunk;
		gfx::Error err;
		while (4 == gfx::read(&_reader, chunk, &err)
			&& err.isOk())
		{
			switch (chunk)
			{
			case BGFX_CHUNK_MAGIC_VB:
			{
				gfx::read(&_reader, data->decl);

				std::uint16_t stride = data->decl.getStride();

				gfx::read(&_reader, numVertices);
				buffers.first.resize(numVertices*stride);
				gfx::read(&_reader, (std::uint8_t*)buffers.first.data(), (std::uint32_t)buffers.first.size());
				
				data->aabb.from_points(buffers.first.data(), numVertices, stride, false);
			}
			break;

			case BGFX_CHUNK_MAGIC_IB:
			{
				gfx::read(&_reader, numIndices);
				buffers.second.resize(numIndices * 2);
				gfx::read(&_reader, (std::uint8_t*)buffers.second.data(), (std::uint32_t)buffers.second.size());
			}
			break;

			case BGFX_CHUNK_MAGIC_IBC:
			{
				gfx::read(&_reader, numIndices);

				std::uint32_t compressedSize;
				gfx::read(&_reader, compressedSize);

				std::vector<uint8_t> compressedIndices(compressedSize);
				gfx::read(&_reader, &compressedIndices[0], compressedSize);

				buffers.second.resize(numIndices * 2);
				ReadBitstream rbs(compressedIndices.data(), compressedSize);
				DecompressIndexBuffer((uint16_t*)buffers.second.data(), numIndices / 3, rbs);
			}
			break;

			case BGFX_CHUNK_MAGIC_PRI:
			{
				uint16_t len;
				gfx::read(&_reader, len);
				Group group;
				group.vertices += numVertices;
				group.indices += numIndices;
				group.primitives += numIndices / 3;
				numVertices = 0;
				numIndices = 0;
				group.material.resize(len);
				gfx::read(&_reader, &group.material[0], len);

				uint16_t num;
				gfx::read(&_reader, num);

				for (std::uint32_t ii = 0; ii < num; ++ii)
				{
					Subset subset;
					gfx::read(&_reader, len);
					subset.name.resize(len);
					gfx::read(&_reader, const_cast<char*>(subset.name.data()), len);

					gfx::read(&_reader, subset.m_startIndex);
					gfx::read(&_reader, subset.m_numIndices);
					gfx::read(&_reader, subset.m_startVertex);
					gfx::read(&_reader, subset.m_numVertices);
					// 					read(_reader, prim.m_sphere);
					// 					read(_reader, prim.m_aabb);
					// 					read(_reader, prim.m_obb);

					group.subsets.push_back(subset);
				}
				data->info.vertices += group.vertices;
				data->info.indices += group.indices;
				data->info.primitives += group.primitives;
				data->groups.emplace_back(group);
				data->buffersMem.emplace_back(buffers);				
				buffers.first.clear();
				buffers.second.clear();
			}
			break;

			default:
				gfx::skip(&_reader, 0);
				break;
			}
		}
	};

	auto createResource = [data, &request, key, absoluteKey]() mutable
	{
		// if someone destroyed our memory
		if (!data)
			return;
		// if nothing was read
		if (data->buffersMem.empty())
			return;

		auto mesh = std::make_shared<Mesh>();
		mesh->decl = data->decl;
		mesh->groups = data->groups;
		mesh->aabb = data->aabb;
		mesh->info = data->info;
		for (std::size_t i = 0; i < mesh->groups.size(); ++i)
		{
			auto& group = mesh->groups[i];
			auto& buffers = data->buffersMem[i];
			const gfx::Memory* memVB = gfx::copy(buffers.first.data(), static_cast<std::uint32_t>(buffers.first.size()));
			buffers.first.clear();
			group.vertexBuffer = std::make_shared<VertexBuffer>();
			group.vertexBuffer->populate(memVB, data->decl);

			const gfx::Memory* memIB = gfx::copy(buffers.second.data(), static_cast<std::uint32_t>(buffers.second.size()));
			buffers.second.clear();
			group.indexBuffer = std::make_shared<IndexBuffer>();
			group.indexBuffer->populate(memIB);

		}
		data.reset();
		request.set_data(key, mesh);
		request.invoke_callbacks();
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
