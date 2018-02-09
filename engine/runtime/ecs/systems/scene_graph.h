#pragma once

#include "../ecs.h"
#include "core/common/basetypes.hpp"
#include <vector>

namespace runtime
{
class scene_graph
{
public:
	scene_graph();
	~scene_graph();
	//-----------------------------------------------------------------------------
	//  Name : frame_update (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void frame_update(delta_t dt);

	//-----------------------------------------------------------------------------
	//  Name : getRoots ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<entity>& get_roots() const
	{
		return _roots;
	}

private:
	/// scene roots
	std::vector<entity> _roots;
};
}
