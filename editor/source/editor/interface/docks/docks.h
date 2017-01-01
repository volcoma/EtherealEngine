#pragma once

#include "../gui_system.h"

class ConsoleLog;

namespace Docks
{
	void renderAssets(ImVec2 area);
	void renderHierarcy(ImVec2 area);
	void renderInspector(ImVec2 area);
	void render_scene(ImVec2 area);
	void renderGame(ImVec2 area);
	void renderStyle(ImVec2 area);
	void renderConsole(ImVec2 area, ConsoleLog& console);
}