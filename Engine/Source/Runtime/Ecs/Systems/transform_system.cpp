#include "transform_system.h"
#include "../components/transform_component.h"
#include "../../system/engine.h"

void updateTransform(core::CHandle<TransformComponent> hTransform, std::chrono::duration<float> dt)
{
	auto pTransform = hTransform.lock();
	if (pTransform)
	{
		pTransform->resolveTransform(true, dt.count());

		auto& children = pTransform->getChildren();
		for (auto& child : children)
		{
			updateTransform(child, dt);
		}
	}


}

void TransformSystem::frame_begin(std::chrono::duration<float> dt)
{
	auto ecs = core::get_subsystem<core::EntityComponentSystem>();
	_roots.clear();
	ecs->each<TransformComponent>([this](core::Entity e, TransformComponent& transformComponent)
	{
		auto parent = transformComponent.getParent();
		if (parent.expired())
		{
			_roots.push_back(transformComponent.handle());
		}
	});

	for (auto& hComponent : _roots)
	{
		updateTransform(hComponent, dt);
	}
}

bool TransformSystem::initialize()
{
	runtime::onFrameBegin.addListener(this, &TransformSystem::frame_begin);

	return true;
}

void TransformSystem::dispose()
{
	runtime::onFrameBegin.removeListener(this, &TransformSystem::frame_begin);
}
