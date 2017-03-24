#pragma once

#include "imguidock.h"

struct HierarchyDock : public ImGuiDock::Dock
{
	HierarchyDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize);

	void render(const ImVec2& area);
};