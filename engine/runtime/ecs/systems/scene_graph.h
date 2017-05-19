#pragma once

#include "../ecs.h"
#include <vector>
#include <chrono>

class transform_component;

namespace runtime
{
	class scene_graph : public core::subsystem
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

		//-----------------------------------------------------------------------------
		//  Name : getRoots ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		const std::vector<chandle<transform_component>>& get_roots() const { return _roots; }
	private:
		/// scene roots
		std::vector<chandle<transform_component>> _roots;
	};
}