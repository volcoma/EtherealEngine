#pragma once

#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bgfx/embedded_shader.h"
#include "bx/spscqueue.h"
#include "bx/handlealloc.h"
#include "bx/readerwriter.h"
#include "bx/error.h"
#include "src/vertexdecl.h"
namespace gfx
{

	using namespace bgfx;
	using namespace bx;
	static bool initted = false;


	struct PosTexCoord0Vertex
	{
		float x, y, z;
		float u, v;

		static void init()
		{
			decl
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();
		}

		static bgfx::VertexDecl decl;
	};


	inline void shutdown()
	{
		if (initted)
			bgfx::shutdown();
	}

	inline bool init(
		RendererType::Enum _type = RendererType::Count
		, uint16_t _vendorId = BGFX_PCI_ID_NONE
		, uint16_t _deviceId = 0
		, CallbackI* _callback = NULL
		, bx::AllocatorI* _reallocator = NULL
	)
	{
		initted = bgfx::init(_type, _vendorId, _deviceId, _callback, _reallocator);

		if (initted)
		{
			PosTexCoord0Vertex::init();
		}

		return initted;
	}	

	void stretchRect(float _destWidth, float _destHeight, float _width = 1.0f, float _height = 1.0f);
}


#include <fstream>

struct FileStreamReaderSeeker : public bx::ReaderSeekerI
{
	FileStreamReaderSeeker(const std::string& path)
	{
		mStream = std::ifstream{ path, std::ios::in | std::ios::binary };
	}
	virtual ~FileStreamReaderSeeker()
	{
	}

	virtual std::int64_t seek(std::int64_t _offset = 0, bx::Whence::Enum _whence = bx::Whence::Current)
	{
		std::ifstream::seekdir way = std::ifstream::cur;
		if (_whence == bx::Whence::Begin)
			way = std::ifstream::beg;
		if (_whence == bx::Whence::End)
			way = std::ifstream::end;

		mStream.seekg(_offset, way);
		return static_cast<std::int64_t>(mStream.tellg());
	}

	virtual std::int32_t read(void* _data, std::int32_t _size, bx::Error* _err)
	{
		mStream.read((char*)_data, _size);

		return static_cast<std::int32_t>(mStream.gcount());
	}

	std::ifstream mStream;
};