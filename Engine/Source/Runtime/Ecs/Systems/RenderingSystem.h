#pragma once

#include "../entityx/System.h"

#include <vector>
using namespace entityx;



class RenderingSystem : public System<RenderingSystem>, public Receiver<System<RenderingSystem>>
{
public:

	virtual void frameRender(EntityManager &entities, EventManager &events, TimeDelta dt);
	void receive(const EntityDestroyedEvent &event);
	void configure(EventManager &events) override
	{
		events.subscribe<EntityDestroyedEvent>(*this);
	}
	struct LodData
	{
		std::uint32_t currentLodIndex = 0;
		std::uint32_t targetLodIndex = 0;
		float currentTime = 0.0f;
	};
private:
	
	std::unordered_map<Entity, std::unordered_map<Entity, LodData>> mLodDataMap;
};
