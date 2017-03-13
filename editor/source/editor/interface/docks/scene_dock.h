#pragma once

#include "imguidock.h"

struct SceneDock : public ImGuiDock::Dock
{
	SceneDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize)
	{
		
		initialize(dtitle, dcloseButton, dminSize, std::bind(&SceneDock::render, this, std::placeholders::_1));
	}

	void render(ImVec2 area);
};