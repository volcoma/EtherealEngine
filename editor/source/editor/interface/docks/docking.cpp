#include "docking.h"
#include "docks.h"
#include "runtime/system/engine.h"
#include "../gui_window.h"
#include "core/logging/logging.h"

bool DockingSystem::initialize()
{
	_docks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	_docks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	_docks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	_docks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	_docks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	_docks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	_docks.emplace_back(std::make_unique<ImGuiDock::Dock>());

	auto& scene = _docks[0];
	scene->initialize("Scene", true, ImVec2(200.0f, 200.0f), &Docks::render_scene);

	auto& game = _docks[1];
	game->initialize("Game", true, ImVec2(200.0f, 200.0f), &Docks::renderGame);

	auto& hierarchy = _docks[2];
	hierarchy->initialize("Hierarchy", true, ImVec2(300.0f, 200.0f), &Docks::renderHierarcy);

	auto& inspector = _docks[3];
	inspector->initialize("Inspector", true, ImVec2(300.0f, 200.0f), &Docks::renderInspector);

	auto& assets = _docks[4];
	assets->initialize("Assets", true, ImVec2(200.0f, 200.0f), &Docks::renderAssets);

	auto& console = _docks[5];
	console->initialize("Console", true, ImVec2(200.0f, 200.0f), [this](ImVec2 area)
	{
		Docks::renderConsole(area, *_console_log.get());
	});

	auto& style = _docks[6];
	style->initialize("Style", true, ImVec2(300.0f, 200.0f), &Docks::renderStyle);

	auto engine = core::get_subsystem<runtime::Engine>();
	const auto& windows = engine->get_windows();
	auto& window = static_cast<GuiWindow&>(*windows[0]);
	auto& dockspace = window.getDockspace();
	dockspace.dock(scene.get(), ImGuiDock::DockSlot::None, 200, true);
	dockspace.dockWith(game.get(), scene.get(), ImGuiDock::DockSlot::Tab, 300, false);
	dockspace.dockWith(inspector.get(), scene.get(), ImGuiDock::DockSlot::Right, 300, true);
	dockspace.dockWith(hierarchy.get(), scene.get(), ImGuiDock::DockSlot::Left, 300, true);
	dockspace.dock(assets.get(), ImGuiDock::DockSlot::Bottom, 200, true);
	dockspace.dockWith(console.get(), assets.get(), ImGuiDock::DockSlot::Tab, 200, true);
	dockspace.dockWith(style.get(), assets.get(), ImGuiDock::DockSlot::Right, 300, true);

	auto logger = logging::get("Log");
	logger->add_sink(_console_log);

	std::function<void()> logVersion = [logger]()
	{
		logger->info() << "Version 1.0";
	};
	_console_log->registerCommand(
		"version",
		"Returns the current version of the Editor.",
		{},
		{},
		logVersion
	);

	return true;
}
