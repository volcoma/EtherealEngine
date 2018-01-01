#pragma once

#include <chrono>

namespace runtime
{
class audio_system
{
public:
	audio_system();
	~audio_system();
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
