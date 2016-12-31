#pragma once

#include "core/ecs.h"
#include <vector>
#include <memory>
#include <chrono>

struct LodData
{
	std::uint32_t currentLodIndex = 0;
	std::uint32_t targetLodIndex = 0;
	float currentTime = 0.0f;
};

class RenderingSystem : public core::Subsystem
{
public:
	//-----------------------------------------------------------------------------
	//  Name : frameRender (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void frame_render(std::chrono::duration<float> dt);
	
	//-----------------------------------------------------------------------------
	//  Name : configure ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void receive(core::Entity e);
	bool initialize() override;
	void dispose() override;
private:
	std::unordered_map<core::Entity, std::unordered_map<core::Entity, LodData>> _lod_data;
};
