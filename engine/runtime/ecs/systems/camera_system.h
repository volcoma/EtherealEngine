#pragma once

#include <chrono>

namespace runtime
{
class camera_system
{
public:
	camera_system();
	~camera_system();
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
