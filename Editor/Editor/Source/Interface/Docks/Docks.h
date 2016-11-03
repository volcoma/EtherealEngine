#pragma once

#include "../GUI.h"

struct ConsoleLog;

namespace Docks
{
	void renderAssets(ImVec2 area);
	void renderHierarcy(ImVec2 area);
	void renderInspector(ImVec2 area);
	void renderScene(ImVec2 area);
	void renderGame(ImVec2 area);
	void renderStyle(ImVec2 area);
	void renderProject(ImVec2 area);
	void renderGettingStarted(ImVec2 area);
	void renderConsole(ImVec2 area, ConsoleLog& console);
}