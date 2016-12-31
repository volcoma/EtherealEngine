#pragma once

#include "core/ecs.h"
#include <vector>
#include <chrono>

class TransformComponent;
class TransformSystem : public core::Subsystem
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

	//-----------------------------------------------------------------------------
	//  Name : getRoots ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<core::CHandle<TransformComponent>>& get_roots() const { return _roots; }
private:
	/// Scene roots
	std::vector<core::CHandle<TransformComponent>> _roots;
};
