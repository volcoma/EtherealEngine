#include "mesh.h"
#include "vertex_buffer.h"
#include "index_buffer.h"

bool Mesh::is_valid() const
{
	if (groups.empty())
	{
		return false;
	}

	for (auto& group : groups)
	{
		if (!group.vertex_buffer)
			return false;

		if (!group.index_buffer)
			return false;
	}
	return true;
}

void Mesh::submit(uint8_t _id, gfx::ProgramHandle _program, const float* _mtx, uint64_t _state) const
{
	if (BGFX_STATE_MASK == _state)
	{
		_state = 0
			| BGFX_STATE_RGB_WRITE
			| BGFX_STATE_ALPHA_WRITE
			| BGFX_STATE_DEPTH_WRITE
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CCW
			| BGFX_STATE_MSAA
			;
	}

	gfx::setTransform(_mtx);
	gfx::setState(_state);

	for (auto it = std::begin(groups), itEnd = std::end(groups); it != itEnd; ++it)
	{
		const auto& group = *it;

		gfx::setIndexBuffer(group.index_buffer->handle);
		gfx::setVertexBuffer(group.vertex_buffer->handle);
		gfx::submit(_id, _program, 0, it != itEnd - 1);
	}
}

