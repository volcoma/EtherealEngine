#pragma once

#include <chrono>
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
	void frame_update(std::chrono::duration<float> dt);
};
}
