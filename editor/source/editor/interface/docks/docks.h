#pragma once

#include "../gui_system.h"

class ConsoleLog;

namespace Docks
{
	void render_assets(ImVec2 area);
	void render_hierarchy(ImVec2 area);
	void render_inspector(ImVec2 area);
	void render_scene(ImVec2 area);
	void render_game(ImVec2 area);
	void render_style(ImVec2 area);
	void render_console(ImVec2 area, ConsoleLog& console);
}