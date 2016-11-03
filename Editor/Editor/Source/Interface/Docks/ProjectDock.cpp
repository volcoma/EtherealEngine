#include "Docks.h"
#include "../../EditorApp.h"
#include "Runtime/Rendering/RenderWindow.h"

namespace Docks
{
	
	void renderProject(ImVec2 area)
	{
		auto& app = Singleton<EditorApp>::getInstance();
		auto& editState = app.getEditState();
		auto& icons = editState.icons;
		if (gui::Button("New"))
		{

		}
		
		gui::SameLine();
		if (gui::Button("Open"))
		{

		}
		gui::Separator();
		auto frame_padding = gui::GetStyle().FramePadding;
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings;
		if (gui::BeginChild("###projects_content", gui::GetContentRegionAvail(), false, flags))
		{

			gui::EndChild();
		}
	}

};