#include "audio_system.h"
#include "../../system/events.h"
#include "../components/audio_listener_component.h"
#include "../components/audio_source_component.h"
#include "../components/transform_component.h"

namespace runtime
{
void audio_system::frame_update(std::chrono::duration<float> dt)
{
	auto& ecs = core::get_subsystem<entity_component_system>();

	ecs.for_each<transform_component, audio_source_component>(
		[](entity e, transform_component& transform, audio_source_component& source) {
			source.update(transform.get_transform());
		});
	ecs.for_each<transform_component, audio_listener_component>(
		[](entity e, transform_component& transform, audio_listener_component& listener) {
			listener.update(transform.get_transform());
		});
}

bool audio_system::initialize()
{
	on_frame_update.connect(this, &audio_system::frame_update);

	return true;
}

void audio_system::dispose()
{
	on_frame_update.disconnect(this, &audio_system::frame_update);
}
}
