#pragma once

#include "imguidock.h"

struct StyleDock : public ImGuiDock::Dock
{
	StyleDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize)
	{
		initialize(dtitle, dcloseButton, dminSize, std::bind(&StyleDock::render, this, std::placeholders::_1));
	}

	void render(ImVec2 area);
};