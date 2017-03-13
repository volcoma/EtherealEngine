#include "inspector_dock.h"
#include "../../edit_state.h"
#include "../inspectors/inspectors.h"

void InspectorDock::render(ImVec2 area)
{
	auto es = core::get_subsystem<editor::EditState>();

	auto& selected = es->selection_data.object;
	if (selected)
	{
		inspect_var(selected);
	}
}
