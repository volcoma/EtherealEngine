#pragma once

#include "imguidock.h"

struct StyleDock : public ImGuiDock::Dock
{
	StyleDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize);

	void render(const ImVec2& area);
};