#pragma once

#include "../gui_system.h"
#include "imguidock.h"
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

struct GameDock : public ImGuiDock::Dock
{
	GameDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize)
	{
		
		initialize(dtitle, dcloseButton, dminSize, std::bind(&GameDock::render, this, std::placeholders::_1));
	}


	void render(ImVec2 area);
};