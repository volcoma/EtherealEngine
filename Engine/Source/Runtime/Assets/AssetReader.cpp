#include "AssetReader.h"
#include "../Rendering/Texture.h"
#include "../Rendering/Uniform.h"
#include "../Rendering/Shader.h"
#include "../Rendering/Mesh.h"
#include "../Rendering/Material.h"
#include "../Rendering/VertexBuffer.h"
#include "../Rendering/IndexBuffer.h"
#include "../System/FileSystem.h"
#include "../System/Application.h"
#include "../Threading/ThreadPool.h"

#include "Core/serialization/archives.h"
#include "Meta/Rendering/Material.hpp"

#include <cstdint>

#include "ib-compress/indexbufferdecompression.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.c"
typedef unsigned char stbi_uc;
extern "C" stbi_uc *stbi_load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp);

void AssetReader::loadTextureFromFile(const std::string& relativeKey, const std::string& absoluteKey, bool async, LoadRequest<Texture>& request)
{
	std::shared_ptr<fs::ByteArray> read_memory = std::make_shared<fs::ByteArray>();

	auto readMemory = [read_memory, absoluteKey]()
	{
		if (!read_memory)
			return;

		*read_memory = fs::readStream(std::ifstream{ absoluteKey, std::ios::in | std::ios::binary });
	};

	auto createResource = [read_memory, relativeKey, absoluteKey, &request]() mutable
	{
		// if someone destroyed our memory
		if (!read_memory)
			return;
		// if nothing was read
		if (read_memory->empty())
			return;

		std::string ext = fs::getFileNameExtension(absoluteKey);
		if (ext == "dds"
			|| ext == "pvr"
			|| ext == "ktx"
			|| ext == "asset")
		{
			const gfx::Memory* mem = gfx::copy(read_memory->data(), static_cast<std::uint32_t>(read_memory->size()));
			read_memory->clear();
			read_memory.reset();

			if (nullptr != mem)
			{
				auto texture = std::make_shared<Texture>(mem, 0, 0, nullptr);
				request.setData(relativeKey, texture);
				request.invokeCallbacks();
			}
		}
		else
		{
			int width = 0;
			int height = 0;
			int comp = 0;

			uint8_t* img = nullptr;
			img = stbi_load_from_memory((uint8_t*)read_memory->data()
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


				free(img);

				request.setData(relativeKey, texture);
				request.invokeCallbacks();
			}
		}
	};

	if (async)
	{
		auto& app = Singleton<Application>::getInstance();
		auto& threadPool = app.getThreadPool();
		auto task = threadPool.enqueue_with_callback(
			// load function
			[readMemory]()
		{
			readMemory();
		},
			// callback to the issuer
			[createResource]() mutable
		{
			createResource();
		});
		request.setTask(task);
	}
	else
	{
		readMemory();
		createResource();
	}

}

void AssetReader::loadShaderFromFile(const std::string& relativeKey, const std::string& absoluteKey, bool async, LoadRequest<Shader>& request)
{

	std::shared_ptr<fs::ByteArray> read_memory = std::make_shared<fs::ByteArray>();

	auto readMemory = [read_memory, absoluteKey]()
	{
		if (!read_memory)
			return;

		*read_memory = fs::readStream(std::ifstream{ absoluteKey, std::ios::in | std::ios::binary });
	};

	auto createResource = [read_memory, &request, relativeKey]() mutable
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

			request.setData(relativeKey, shader);
			request.invokeCallbacks();

		}
	};

	if (async)
	{
		auto& app = Singleton<Application>::getInstance();
		auto& threadPool = app.getThreadPool();
		auto task = threadPool.enqueue_with_callback(
			// load function
			[readMemory]()
		{
			readMemory();
		},
			// callback to the issuer
			[createResource]() mutable
		{
			createResource();
		});

		request.setTask(task);
	}
	else
	{
		readMemory();
		createResource();
	}
}

void AssetReader::loadShaderFromMemory(const std::string& relativeKey, const std::uint8_t* data, std::uint32_t size, LoadRequest<Shader>& request)
{
	auto createResource = [&relativeKey, data, size, &request]() mutable
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

			request.setData(relativeKey, shader);
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
};


void AssetReader::loadMeshFromFile(const std::string& relativeKey, const std::string& absoluteKey, bool async, LoadRequest<Mesh>& request)
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

		FileStreamReaderSeeker _reader(absoluteKey);

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
				data->aabb.fromPoints(buffers.first.data(), numVertices, stride, true);
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
					Primitive prim;
					gfx::read(&_reader, len);
					prim.name.resize(len);
					gfx::read(&_reader, const_cast<char*>(prim.name.data()), len);

					gfx::read(&_reader, prim.m_startIndex);
					gfx::read(&_reader, prim.m_numIndices);
					gfx::read(&_reader, prim.m_startVertex);
					gfx::read(&_reader, prim.m_numVertices);
					// 					read(_reader, prim.m_sphere);
					// 					read(_reader, prim.m_aabb);
					// 					read(_reader, prim.m_obb);

					group.primitives.push_back(prim);
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

	auto createResource = [data, &request, relativeKey]() mutable
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
		request.setData(relativeKey, mesh);
		request.invokeCallbacks();
	};

	if (async)
	{
		auto& application = Singleton<Application>::getInstance();
		auto& threadPool = application.getThreadPool();
		auto task = threadPool.enqueue_with_callback(
			// load function
			[readMemory]()
		{
			readMemory();
		},
			// callback to the issuer
			[createResource]() mutable
		{
			createResource();
		});

		request.setTask(task);
	}
	else
	{
		readMemory();
		createResource();
	}
}

void AssetReader::loadMaterialFromFile(const std::string& relativeKey, const std::string& absoluteKey, bool async, LoadRequest<Material>& request)
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

	auto createResource = [matWrapper, relativeKey, request]() mutable
	{
		request.setData(relativeKey, matWrapper->hMaterial);
		request.invokeCallbacks();
	};

	if (async)
	{
		auto& application = Singleton<Application>::getInstance();
		auto& threadPool = application.getThreadPool();
		auto task = threadPool.enqueue_with_callback(
			// load function
			[deserialize]() mutable
		{
			deserialize();
		},
			// callback to the issuer
			[createResource]() mutable
		{
			createResource();
		});

		request.setTask(task);
	}
	else
	{
		deserialize();
		createResource();
	}
}
