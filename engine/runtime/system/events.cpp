#include "events.h"

namespace runtime
{
event<void(delta_t)> on_frame_begin;
event<void(delta_t)> on_frame_update;
event<void(delta_t)> on_frame_render;
event<void(delta_t)> on_frame_end;

event<void(const std::pair<std::uint32_t, bool>&, const std::vector<mml::platform_event>&)>
	on_platform_events;
}
