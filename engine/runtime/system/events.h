#pragma once

#include "../rendering/render_window.h"
#include <chrono>
#include <cstdint>

namespace runtime
{
/// engine loop events
extern event<void(std::chrono::duration<float>)> on_frame_begin;
extern event<void(std::chrono::duration<float>)> on_frame_update;
extern event<void(std::chrono::duration<float>)> on_frame_render;
extern event<void(std::chrono::duration<float>)> on_frame_end;

/// platform events
extern event<void(const std::pair<std::uint32_t, bool>&, const std::vector<mml::platform_event>&)> on_platform_events;
}
