#pragma once

#include "imguidock.h"

struct inspector_dock : public imguidock::dock
{
	inspector_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size);

	void render(const ImVec2& area);
};
