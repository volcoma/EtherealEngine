#pragma once

#include "core/subsystem.h"
#include <chrono>

class CameraSystem : public core::Subsystem
{
public:
	bool initialize();
	void dispose();
	//-----------------------------------------------------------------------------
	//  Name : frame_begin (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void frame_begin(std::chrono::duration<float> dt);
};
