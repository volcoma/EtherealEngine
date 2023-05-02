#include "reflection_probe_system.h"
#include "../../system/events.h"
#include "../components/reflection_probe_component.h"

#include <core/system/subsystem.h>

namespace runtime
{
void reflection_probe_system::frame_update(delta_t dt)
{
    auto& ecs = core::get_subsystem<entity_component_system>();

    ecs.for_each<reflection_probe_component>(
        [](entity e, reflection_probe_component& probe)
        {
            probe.update();
        });
}

reflection_probe_system::reflection_probe_system()
{
    on_frame_update.connect(this, &reflection_probe_system::frame_update);
}

reflection_probe_system::~reflection_probe_system()
{
    on_frame_update.disconnect(this, &reflection_probe_system::frame_update);
}
} // namespace runtime
