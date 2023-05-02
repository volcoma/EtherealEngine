#include "index_buffer.h"
namespace gfx
{

index_buffer::index_buffer(const memory_view* _mem, std::uint16_t _flags /*= BGFX_BUFFER_NONE*/)
{
    handle = create_index_buffer(_mem, _flags);
}
} // namespace gfx
