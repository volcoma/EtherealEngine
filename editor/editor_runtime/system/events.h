#pragma once

#include <core/filesystem/filesystem.h>
#include <core/signals/event.hpp>

#include <chrono>
#include <cstdint>

namespace editor
{
extern hpp::event<void(const fs::path&)> on_open_project;
extern hpp::event<void()> on_close_project;
}
