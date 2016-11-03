#include "CameraSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/CameraComponent.h"


void CameraSystem::frameBegin(ecs::EntityManager &entities, ecs::EventManager &events, ecs::TimeDelta dt)
{

	entities.each<TransformComponent, CameraComponent>([this](
		ecs::Entity e,
		TransformComponent& transformComponent,
		CameraComponent& cameraComponent
		)
	{
		cameraComponent.updateInternal(transformComponent.getTransform());
	});

}
