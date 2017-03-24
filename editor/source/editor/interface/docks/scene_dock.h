#pragma once

#include "imguidock.h"

struct SceneDock : public ImGuiDock::Dock
{
	SceneDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize);

	void render(const ImVec2& area);
};