#include "camera_system.h"
#include "../components/transform_component.h"
#include "../components/camera_component.h"
#include "../../system/engine.h"

namespace runtime
{
	void CameraSystem::frame_begin(std::chrono::duration<float> dt)
	{
		auto ecs = core::get_subsystem<EntityComponentSystem>();

		ecs->each<TransformComponent, CameraComponent>([this](
			Entity e,
			TransformComponent& transformComponent,
			CameraComponent& cameraComponent
			)
		{
			cameraComponent.update(transformComponent.get_transform());
		});

	}

	bool CameraSystem::initialize()
	{
		on_frame_begin.addListener(this, &CameraSystem::frame_begin);

		return true;
	}

	void CameraSystem::dispose()
	{
		on_frame_begin.removeListener(this, &CameraSystem::frame_begin);
	}

}