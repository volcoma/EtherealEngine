#include "Mesh.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

bool Mesh::isValid() const
{
	if (groups.empty())
	{
		return false;
	}

	for (auto& group : groups)
	{
		if (!group.vertexBuffer)
			return false;

		if (!group.indexBuffer)
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

	for (auto it = groups.begin(), itEnd = groups.end(); it != itEnd; ++it)
	{
		const auto& group = *it;

		gfx::setIndexBuffer(group.indexBuffer->handle);
		gfx::setVertexBuffer(group.vertexBuffer->handle);
		gfx::submit(_id, _program, 0, it != itEnd - 1);
	}
}