#pragma once

#include "imguidock.h"

struct scene_dock : public imguidock::dock
{
	scene_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size);

	void render(const ImVec2& area);
	void show_statistics(const ImVec2& area, unsigned int fps, bool& show_gbuffer);
	bool show_gbuffer = false;
    bool enable_profiler = false;
};
