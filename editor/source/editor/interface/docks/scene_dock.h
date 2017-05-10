#pragma once

#include "imguidock.h"

struct scene_dock : public imguidock::dock
{
	scene_dock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize);

	void render(const ImVec2& area);
};