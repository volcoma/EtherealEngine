#pragma once

#include "../ecs.h"

namespace runtime
{
class bone_system : public core::subsystem
{
public:
	bool initialize();
	void dispose();
	//-----------------------------------------------------------------------------
	//  Name : frame_update (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void frame_update(std::chrono::duration<float> dt);
};
}
