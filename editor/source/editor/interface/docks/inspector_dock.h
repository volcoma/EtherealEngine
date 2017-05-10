#pragma once

#include "imguidock.h"

struct inspector_dock : public imguidock::dock
{
	inspector_dock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize);

	void render(const ImVec2& area);
};