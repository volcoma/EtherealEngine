#include "docks.h"
#include "../../edit_state.h"
#include "../inspectors/inspectors.h"

namespace Docks
{

	void render_inspector(ImVec2 area)
	{
		auto es = core::get_subsystem<editor::EditState>();
		
		auto& selected = es->selection_data.object;
		if (selected)
		{
			inspect_var(selected);
		}
	}
};