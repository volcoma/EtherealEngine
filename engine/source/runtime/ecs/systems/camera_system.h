#pragma once

#include "core/subsystem/subsystem.h"
#include <chrono>
namespace runtime
{
	class CameraSystem : public core::Subsystem
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