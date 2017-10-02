#include "camera_system.h"
#include "../../system/events.h"
#include "../components/camera_component.h"
#include "../components/transform_component.h"

namespace runtime
{
void camera_system::frame_update(std::chrono::duration<float> dt)
{
	auto& ecs = core::get_subsystem<entity_component_system>();

	ecs.each<transform_component, camera_component>(
		[this](entity e, transform_component& transformComponent, camera_component& cameraComponent) {
			cameraComponent.update(transformComponent.get_transform());
		});
}

bool camera_system::initialize()
{
	on_frame_update.connect(this, &camera_system::frame_update);

	return true;
}

void camera_system::dispose()
{
	on_frame_update.disconnect(this, &camera_system::frame_update);
}
}
