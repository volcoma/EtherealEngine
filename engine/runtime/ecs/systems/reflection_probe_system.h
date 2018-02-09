#pragma once

#include "core/common/basetypes.hpp"

namespace runtime
{
class reflection_probe_system
{
public:
	reflection_probe_system();
	~reflection_probe_system();
	//-----------------------------------------------------------------------------
	//  Name : frame_update (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void frame_update(delta_t dt);
};
}
