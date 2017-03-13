#pragma once

#include "imguidock.h"

struct InspectorDock : public ImGuiDock::Dock
{
	InspectorDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize)
	{
		
		initialize(dtitle, dcloseButton, dminSize, std::bind(&InspectorDock::render, this, std::placeholders::_1));
	}


	void render(ImVec2 area);
};