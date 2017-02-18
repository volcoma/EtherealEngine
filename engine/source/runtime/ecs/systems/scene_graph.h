#pragma once

#include "../ecs.h"
#include <vector>
#include <chrono>

class TransformComponent;

namespace runtime
{
	class SceneGraph : public core::Subsystem
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
		const std::vector<CHandle<TransformComponent>>& get_roots() const { return _roots; }
	private:
		/// Scene roots
		std::vector<CHandle<TransformComponent>> _roots;
	};
}