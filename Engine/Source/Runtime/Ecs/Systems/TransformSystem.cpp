#include "TransformSystem.h"
#include "../Components/TransformComponent.h"

void updateTransform(ComponentHandle<TransformComponent> hTransform, TimeDelta dt)
{
	auto transform = hTransform.lock();
	if (transform)
	{
		transform->resolveTransform(true, dt);

		auto& children = transform->getChildren();
		for (auto& child : children)
		{
			updateTransform(child, dt);
		}
	}
	

}

void TransformSystem::frameBegin(EntityManager &entities, EventManager &events, TimeDelta dt)
{
	mRoots.clear();
	entities.each<TransformComponent>([this](Entity e, TransformComponent& transformComponent)
	{
		auto parent = transformComponent.getParent();
		if (parent.expired())
		{
			mRoots.push_back(transformComponent.makeHandle());
		}
	});

	for (auto& hComponent : mRoots)
	{
		updateTransform(hComponent, dt);
	}
}
