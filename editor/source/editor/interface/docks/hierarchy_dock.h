#pragma once

#include "imguidock.h"

struct HierarchyDock : public ImGuiDock::Dock
{
	HierarchyDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize)
	{	
		initialize(dtitle, dcloseButton, dminSize, std::bind(&HierarchyDock::render, this, std::placeholders::_1));
	}

	void render(ImVec2 area);
};