#pragma once

#include "imguidock.h"
#include "runtime/ecs/ecs.h"

struct hierarchy_dock : public imguidock::dock
{
	hierarchy_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size);

	void render(const ImVec2& area);
    
    void draw_entity(runtime::entity entity);
    
    bool edit_label = false;
    ImGuiID id = 0;    
};
