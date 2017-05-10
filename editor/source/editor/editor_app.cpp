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

	void editor_app::start()
	{
		auto engine = core::get_subsystem<runtime::engine>();

		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		desktop.width = 1280;
		desktop.height = 720;
		auto main_window = std::make_shared<main_editor_window>(
			desktop,
			"Editor",
			sf::Style::Default);

		if (!engine->start(main_window))
		{
			_exitcode = -1;
			return;
		}

		core::add_subsystem<GuiSystem>();
		core::add_subsystem<docking_system>();
		core::add_subsystem<editor_state>();
		core::add_subsystem<picking_system>();
		core::add_subsystem<debugdraw_system>();
		core::add_subsystem<project_manager>();
	}
}