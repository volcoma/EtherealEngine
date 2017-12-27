#include "reflection_probe_system.h"
#include "../../system/events.h"
#include "../components/reflection_probe_component.h"

namespace runtime
{
void reflection_probe_system::frame_update(std::chrono::duration<float> dt)
{
	auto& ecs = core::get_subsystem<entity_component_system>();

	ecs.for_each<reflection_probe_component>(
		[](entity e, reflection_probe_component& probe) {
			probe.update();
		});
}

bool reflection_probe_system::initialize()
{
	on_frame_update.connect(this, &reflection_probe_system::frame_update);

	return true;
}

void reflection_probe_system::dispose()
{
	on_frame_update.disconnect(this, &reflection_probe_system::frame_update);
}
}
