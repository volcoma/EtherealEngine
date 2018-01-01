#pragma once

#include <chrono>

namespace runtime
{
class bone_system
{
public:
	bone_system();
	~bone_system();
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
