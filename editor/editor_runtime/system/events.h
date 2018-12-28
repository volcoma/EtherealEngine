#pragma once

#include <core/filesystem/filesystem.h>
#include <core/signals/event.hpp>

#include <chrono>
#include <cstdint>

namespace editor
{
extern event<void(const fs::path&)> on_open_project;
extern event<void()> on_close_project;
}
