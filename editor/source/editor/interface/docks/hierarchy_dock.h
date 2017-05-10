#pragma once

#include "imguidock.h"

struct hierarchy_dock : public imguidock::dock
{
	hierarchy_dock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize);

	void render(const ImVec2& area);
};