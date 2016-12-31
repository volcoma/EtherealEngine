#include "docks.h"
#include "../../edit_state.h"
#include "../inspectors/inspectors.h"

namespace Docks
{

	void renderInspector(ImVec2 area)
	{
		auto es = core::get_subsystem<EditState>();
		
		auto& selected = es->selectionData.object;
		if (selected)
		{
			inspect_var(selected);
		}
	}
};