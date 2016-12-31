#include "camera_system.h"
#include "../components/transform_component.h"
#include "../components/camera_component.h"
#include "../../system/engine.h"

void CameraSystem::frame_begin(std::chrono::duration<float> dt)
{
	auto ecs = core::get_subsystem<core::EntityComponentSystem>();

	ecs->each<TransformComponent, CameraComponent>([this](
		core::Entity e,
		TransformComponent& transformComponent,
		CameraComponent& cameraComponent
		)
	{
		cameraComponent.updateInternal(transformComponent.getTransform());
	});

}

bool CameraSystem::initialize()
{
	runtime::on_frame_begin.addListener(this, &CameraSystem::frame_begin);

	return true;
}

void CameraSystem::dispose()
{
	runtime::on_frame_begin.removeListener(this, &CameraSystem::frame_begin);
}

