#include "Docks.h"
#include "../../EditorApp.h"
#include "../Inspectors/Inspectors.h"

namespace Docks
{

	void renderInspector(ImVec2 area)
	{
		auto& app = Singleton<EditorApp>::getInstance();
		auto& editState = app.getEditState();
		
		auto& selected = editState.selected;
		if (selected)
		{
			inspectVar(selected);
		}
	}
};