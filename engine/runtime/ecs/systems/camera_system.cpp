#include "camera_system.h"
#include "../../system/events.h"
#include "../components/camera_component.h"
#include "../components/transform_component.h"

#include <core/system/subsystem.h>

namespace runtime
{
void camera_system::frame_update(delta_t)
{
	auto& ecs = core::get_subsystem<entity_component_system>();

	ecs.for_each<transform_component, camera_component>(
		[](entity e, transform_component& transform, camera_component& camera) {
			camera.update(transform.get_transform());
		});
}

camera_system::camera_system()
{
	on_frame_update.connect(this, &camera_system::frame_update);
}

camera_system::~camera_system()
{
	on_frame_update.disconnect(this, &camera_system::frame_update);
}
}
