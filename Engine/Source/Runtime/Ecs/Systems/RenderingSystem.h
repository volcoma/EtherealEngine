#pragma once

#include "../entityx/System.h"
#include <vector>
#include <memory>

using namespace entityx;

struct LodData
{
	std::uint32_t currentLodIndex = 0;
	std::uint32_t targetLodIndex = 0;
	float currentTime = 0.0f;
};

class RenderingSystem : public System<RenderingSystem>, public Receiver<System<RenderingSystem>>
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
	virtual void frameRender(EntityManager &entities, EventManager &events, TimeDelta dt) override;
	
	//-----------------------------------------------------------------------------
	//  Name : receive ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void receive(const EntityDestroyedEvent &event);
	
	//-----------------------------------------------------------------------------
	//  Name : configure ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void configure(EventManager &events) override;

private:
	std::unordered_map<Entity, std::unordered_map<Entity, LodData>> mLodDataMap;
};
