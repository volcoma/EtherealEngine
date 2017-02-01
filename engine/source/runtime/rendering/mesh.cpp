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
