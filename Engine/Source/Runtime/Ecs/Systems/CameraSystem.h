#pragma once

#include "Runtime/Ecs/World.h"

class CameraSystem : public ecs::System<CameraSystem>
{
public:
	//-----------------------------------------------------------------------------
	//  Name : frameBegin (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameBegin(ecs::EntityManager &entities, ecs::EventManager &events, ecs::TimeDelta dt);
};
