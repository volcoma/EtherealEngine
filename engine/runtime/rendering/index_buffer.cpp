#include "index_buffer.h"

index_buffer::~index_buffer()
{
	dispose();
}

void index_buffer::dispose()
{
	if(is_valid())
		gfx::destroyIndexBuffer(handle);

	handle = {bgfx::kInvalidHandle};
}

bool index_buffer::is_valid() const
{
	return gfx::isValid(handle);
}

void index_buffer::populate(const gfx::Memory* _mem, std::uint16_t _flags /*= BGFX_BUFFER_NONE*/)
{
	dispose();
	handle = gfx::createIndexBuffer(_mem, _flags);
}
