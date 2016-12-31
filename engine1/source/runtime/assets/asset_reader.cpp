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
#include "core/task.h"
#include "core/serialization/archives.h"
#include "meta/rendering/material.hpp"

#include <cstdint>

#include "ib-compress/indexbufferdecompression.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.c"
typedef unsigned char stbi_uc;
extern "C" stbi_uc *stbi_load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp);
extern "C" void stbi_image_free(void *retval_from_stbi_load);

void AssetReader::loadTextureFromFile(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Texture>& request)
{
	std::shared_ptr<fs::ByteArray> read_memory = std::make_shared<fs::ByteArray>();

	auto readMemory = [read_memory, absoluteKey]()
	{
		if (!read_memory)
			return;

		*read_memory = fs::read_stream(std::ifstream{ absoluteKey, std::ios::in | std::ios::binary });
	};

	auto createResource = [read_memory, key, absoluteKey, &request]() mutable
	{
		// if someone destroyed our memory
		if (!read_memory)
			return;
		// if nothing was read
		if (read_memory->empty())
			return;

		std::string ext = absoluteKey.extension().string();
		if (ext == ".dds"
			|| ext == ".pvr"
			|| ext == ".ktx"
			|| ext == ".asset")
		{
			const gfx::Memory* mem = gfx::copy(read_memory->data(), static_cast<std::uint32_t>(read_memory->size()));
			read_memory->clear();
			read_memory.reset();

			if (nullptr != mem)
			{
				auto texture = std::make_shared<Texture>(mem, 0, 0, nullptr);
				request.setData(key, texture);
				request.invokeCallbacks();
			}
		}
		else
		{
			int width = 0;
			int height = 0;
			int comp = 0;

			uint8_t* img = stbi_load_from_memory(read_memory->data()
				, static_cast<int>(read_memory->size())
				, &width
				, &height
				, &comp
				, 4
			);

			read_memory->clear();
			read_memory.reset();

			if (nullptr != img)
			{
				auto texture = std::make_shared<Texture>(
					std::uint16_t(width)
					, std::uint16_t(height)
					, false
					, 1
					, gfx::TextureFormat::RGBA8
					, 0
					, gfx::copy(img, width*height * 4)
					);


				stbi_image_free(img);

				request.setData(key, texture);
				request.invokeCallbacks();
			}
		}
	};

	if (async)
	{
		auto ts = core::get_subsystem<core::TaskSystem>();

		auto task = ts->create("", [ts, readMemory, createResource]()
		{
			readMemory();

			auto callback = ts->create("Create Resource", createResource);

			ts->run_on_main(callback);
		});
		request.setTask(task);
		ts->run(task);
	}
	else
	{
		readMemory();
		createResource();
	}

}

void AssetReader::loadShaderFromFile(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Shader>& request)
{

	std::shared_ptr<fs::ByteArray> read_memory = std::make_shared<fs::ByteArray>();

	auto readMemory = [read_memory, absoluteKey]()
	{
		if (!read_memory)
			return;

		*read_memory = fs::read_stream(std::ifstream{ absoluteKey, std::ios::in | std::ios::binary });
	};

	auto createResource = [read_memory, &request, key]() mutable
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

			request.setData(key, shader);
			request.invokeCallbacks();

		}
	};

	if (async)
	{
		auto ts = core::get_subsystem<core::TaskSystem>();

		auto task = ts->create("", [ts, readMemory, createResource]()
		{
			readMemory();

			auto callback = ts->create("Create Resource", createResource);

			ts->run_on_main(callback);
		});
		request.setTask(task);
		ts->run(task);
	}
	else
	{
		readMemory();
		createResource();
	}
}

void AssetReader::loadShaderFromMemory(const std::string& key, const std::uint8_t* data, std::uint32_t size, LoadRequest<Shader>& request)
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

			request.setData(key, shader);
			request.invokeCallbacks();

		}
	};

	createResource();

}

struct MeshData
{
	gfx::VertexDecl decl;
	std::vector<Group> groups;
	std::vector<std::pair<fs::ByteArray, fs::ByteArray>> buffersMem; // vb, ib
	math::bbox aabb;
	MeshInfo info;
};


void AssetReader::loadMeshFromFile(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Mesh>& request)
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

		std::pair<fs::ByteArray, fs::ByteArray> buffers;
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

				std::uint16_t numVertices;
				gfx::read(&_reader, numVertices);
				buffers.first.resize(numVertices*stride);
				gfx::read(&_reader, (std::uint8_t*)buffers.first.data(), (std::uint32_t)buffers.first.size());
				
				data->aabb.fromPoints(buffers.first.data(), numVertices, stride, false);
			}
			break;

			case BGFX_CHUNK_MAGIC_IB:
			{
				std::uint32_t numIndices;
				gfx::read(&_reader, numIndices);
				buffers.second.resize(numIndices * 2);
				gfx::read(&_reader, (std::uint8_t*)buffers.second.data(), (std::uint32_t)buffers.second.size());
			}
			break;

			case BGFX_CHUNK_MAGIC_IBC:
			{
				std::uint32_t numIndices;
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
					data->info.vertices += subset.m_numVertices;
					data->info.indices += subset.m_numIndices;
					data->info.primitives += subset.m_numIndices / 3;
				}

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

	auto createResource = [data, &request, key]() mutable
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
		request.setData(key, mesh);
		request.invokeCallbacks();
	};

	if (async)
	{
		auto ts = core::get_subsystem<core::TaskSystem>();

		auto task = ts->create("", [ts, readMemory, createResource]()
		{
			readMemory();

			auto callback = ts->create("Create Resource", createResource);

			ts->run_on_main(callback);
		});
		request.setTask(task);
		ts->run(task);
	}
	else
	{
		readMemory();
		createResource();
	}
}

void AssetReader::loadMaterialFromFile(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Material>& request)
{
	struct MatWrapper
	{
		std::shared_ptr<Material> hMaterial;
	};
	auto hMaterial = std::make_shared<Material>();
	auto matWrapper = std::make_shared<MatWrapper>();
	matWrapper->hMaterial = hMaterial;
	auto deserialize = [matWrapper, absoluteKey, request]() mutable
	{
		std::ifstream stream{ absoluteKey, std::ios::in | std::ios::binary };
		cereal::IArchive_JSON ar(stream);

		ar(
			cereal::make_nvp("material", matWrapper->hMaterial)
		);
	};

	auto createResource = [matWrapper, key, request]() mutable
	{
		request.setData(key, matWrapper->hMaterial);
		request.invokeCallbacks();
	};

	if (async)
	{
		auto ts = core::get_subsystem<core::TaskSystem>();

		auto task = ts->create("", [ts, deserialize, createResource]() mutable
		{
			deserialize();

			auto callback = ts->create("Create Resource", createResource);

			ts->run_on_main(callback);
		});
		request.setTask(task);
		ts->run(task);
	}
	else
	{
		deserialize();
		createResource();
	}
}

void AssetReader::loadPrefabFromFile(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Prefab>& request)
{

	std::shared_ptr<std::istringstream> read_memory = std::make_shared<std::istringstream>();

	auto readMemory = [read_memory, absoluteKey]()
	{
		if (!read_memory)
			return;

		auto mem = fs::read_stream(std::fstream{ absoluteKey, std::fstream::in | std::fstream::out | std::ios::binary });
 		*read_memory = std::istringstream(std::string(reinterpret_cast<const char*>(mem.data()), mem.size()));
	};

	auto createResource = [read_memory, key, request]() mutable
	{
		auto prefab = std::make_shared<Prefab>();
		prefab->data = read_memory;
		request.setData(key, prefab);
		request.invokeCallbacks();
	};
	

	if (async)
	{
		auto ts = core::get_subsystem<core::TaskSystem>();

		auto task = ts->create("", [ts, readMemory, createResource]() mutable
		{
			readMemory();

			auto callback = ts->create("Create Resource", createResource);

			ts->run_on_main(callback);
		});
		request.setTask(task);
		ts->run(task);
	}
	else
	{
		readMemory();
		createResource();
	}
}
