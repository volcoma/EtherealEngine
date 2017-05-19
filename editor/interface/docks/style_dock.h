#pragma once

#include "imguidock.h"

struct style_dock : public imguidock::dock
{
	style_dock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize);

	void render(const ImVec2& area);
};