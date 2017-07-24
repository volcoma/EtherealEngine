#include "vertex_buffer.h"

vertex_buffer::~vertex_buffer()
{
	dispose();
}

void vertex_buffer::dispose()
{
	if(is_valid())
		gfx::destroyVertexBuffer(handle);

	handle = {bgfx::kInvalidHandle};
}

bool vertex_buffer::is_valid() const
{
	return gfx::isValid(handle);
}

void vertex_buffer::populate(const gfx::Memory* _mem, const gfx::VertexDecl& _decl,
							 std::uint16_t _flags /*= BGFX_BUFFER_NONE*/)
{
	dispose();

	handle = gfx::createVertexBuffer(_mem, _decl, _flags);
}
