#pragma once

#include "core/common/basetypes.hpp"
#include "core/signals/event.hpp"
#include <chrono>
#include <cstdint>
#include <mml/window/event.hpp>

namespace runtime
{
/// engine loop events
extern event<void(delta_t)> on_frame_begin;
extern event<void(delta_t)> on_frame_update;
extern event<void(delta_t)> on_frame_render;
extern event<void(delta_t)> on_frame_end;

/// platform events

/// <window_id, is_focused>, {events}
extern event<void(const std::pair<std::uint32_t, bool>&, const std::vector<mml::platform_event>&)>
	on_platform_events;
}
