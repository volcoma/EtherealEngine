#include "vertex_buffer.h"

namespace gfx
{

vertex_buffer::vertex_buffer(const memory_view* _mem,
                             const vertex_layout& _decl,
                             std::uint16_t _flags /*= BGFX_BUFFER_NONE*/)
{
    handle = create_vertex_buffer(_mem, _decl, _flags);
}
} // namespace gfx
