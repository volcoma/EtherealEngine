#include "VertexBuffer.h"

VertexBuffer::~VertexBuffer()
{
	dispose();
}

void VertexBuffer::dispose()
{
	if (isValid())
		gfx::destroyVertexBuffer(handle);
}

bool VertexBuffer::isValid() const
{
	return gfx::isValid(handle);
}

void VertexBuffer::populate(const gfx::Memory* _mem, const gfx::VertexDecl& _decl, std::uint16_t _flags /*= BGFX_BUFFER_NONE*/)
{
	dispose();

	handle = gfx::createVertexBuffer(_mem, _decl, _flags);
}
