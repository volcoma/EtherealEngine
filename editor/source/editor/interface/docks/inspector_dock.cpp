#include "inspector_dock.h"
#include "../../edit_state.h"
#include "../inspectors/inspectors.h"

void InspectorDock::render(const ImVec2& area)
{
	auto es = core::get_subsystem<editor::EditState>();

	auto& selected = es->selection_data.object;
	if (selected)
	{
		inspect_var(selected);
	}
}

InspectorDock::InspectorDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize)
{

	initialize(dtitle, dcloseButton, dminSize, std::bind(&InspectorDock::render, this, std::placeholders::_1));
}
