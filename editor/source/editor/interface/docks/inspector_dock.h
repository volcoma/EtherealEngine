#pragma once

#include "imguidock.h"

struct InspectorDock : public ImGuiDock::Dock
{
	InspectorDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize);


	void render(const ImVec2& area);
};