#include "events.h"

namespace editor
{
hpp::event<void(const fs::path&)> on_open_project;
hpp::event<void()> on_close_project;
}
