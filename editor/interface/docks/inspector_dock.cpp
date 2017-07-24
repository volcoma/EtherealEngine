#include "inspector_dock.h"
#include "../../editing/editing_system.h"
#include "../inspectors/inspectors.h"

void inspector_dock::render(const ImVec2&)
{
	auto& es = core::get_subsystem<editor::editing_system>();

	auto& selected = es.selection_data.object;
	if (selected)
	{
		inspect_var(selected);
	}
}

inspector_dock::inspector_dock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize)
{

	initialize(dtitle, dcloseButton, dminSize, std::bind(&inspector_dock::render, this, std::placeholders::_1));
}
