#pragma once

#include "../gui_system.h"
#include "imguidock.h"

class console_log;
struct game_dock : public imguidock::dock
{
	game_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size);

	void render(const ImVec2& area);
};
