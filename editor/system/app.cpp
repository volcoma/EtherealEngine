#include "app.h"
#include "runtime/system/engine.h"
#include "runtime/rendering/render_window.h"
#include "editor_window.h"
#include "../interface/gui_system.h"
#include "../interface/docks/docking.h"
#include "../editing/editing_system.h"
#include "../system/project_manager.h"
#include "../editing/picking_system.h"
#include "../rendering/debugdraw_system.h"

namespace editor
{

	void app::start()
	{
		auto& engine = core::get_subsystem<runtime::engine>();

		mml::video_mode desktop = mml::video_mode::get_desktop_mode();
		desktop.width = 1280;
		desktop.height = 720;
		auto main_window = std::make_shared<main_editor_window>(
			desktop,
			"Editor",
			mml::style::Default);

		if (!engine.start(main_window))
		{
			_exitcode = -1;
			return;
		}

		core::add_subsystem<gui_system>();
		core::add_subsystem<docking_system>();
		core::add_subsystem<editing_system>();
		core::add_subsystem<picking_system>();
		core::add_subsystem<debugdraw_system>();
		core::add_subsystem<project_manager>();
	}
}