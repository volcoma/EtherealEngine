#include "inspector_dock.h"
#include "../../editing/editing_system.h"
#include "../inspectors/inspectors.h"
#include "core/system/subsystem.h"

void inspector_dock::render(const ImVec2&)
{
	auto& es = core::get_subsystem<editor::editing_system>();

	auto& selected = es.selection_data.object;
	if(selected)
	{
		inspect_var(selected);
	}
}

inspector_dock::inspector_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size)
{

	initialize(dtitle, close_button, min_size,
			   std::bind(&inspector_dock::render, this, std::placeholders::_1));
}
