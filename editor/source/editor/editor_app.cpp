#include "editor_app.h"
#include "runtime/system/engine.h"
#include "runtime/rendering/render_window.h"
#include "editor_window.h"
#include "interface/gui_system.h"
#include "interface/docks/docking.h"
#include "edit_state.h"
#include "project.h"
#include "systems/picking_system.h"
#include "systems/debugdraw_system.h"

namespace editor
{

	void EditorApp::start()
	{
		auto engine = core::get_subsystem<runtime::Engine>();

		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		desktop.width = 1280;
		desktop.height = 720;
		auto main_window = std::make_shared<MainEditorWindow>(
			desktop,
			"Editor",
			sf::Style::Default);

		if (!engine->start(main_window))
			_exitcode = -1;

		core::add_subsystem<GuiSystem>();
		core::add_subsystem<DockingSystem>();
		core::add_subsystem<EditState>();
		core::add_subsystem<PickingSystem>();
		core::add_subsystem<DebugDrawSystem>();

		auto pm = core::add_subsystem<ProjectManager>();
		pm->open();
	}


}