#include "events.h"

namespace editor
{
event<void(const fs::path&)> on_open_project;
event<void()> on_close_project;
}
