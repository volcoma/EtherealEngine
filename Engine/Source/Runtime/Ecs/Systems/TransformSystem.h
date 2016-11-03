#pragma once

#include "../entityx/System.h"
#include <vector>
using namespace entityx;

class TransformComponent;
class TransformSystem : public System<TransformSystem>
{
public:
	virtual void frameBegin(EntityManager &entities, EventManager &events, TimeDelta dt);

	const std::vector<ComponentHandle<TransformComponent>>& getRoots() const { return mRoots;}
private:
	std::vector<ComponentHandle<TransformComponent>> mRoots;
};
