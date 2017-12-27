#pragma once

#include "core/system/subsystem.h"
#include <chrono>
namespace runtime
{
class reflection_probe_system : public core::subsystem
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
