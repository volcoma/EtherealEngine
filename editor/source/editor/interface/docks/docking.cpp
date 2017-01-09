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
	game->initialize("Game", true, ImVec2(200.0f, 200.0f), &Docks::render_game);

	auto& hierarchy = _docks[2];
	hierarchy->initialize("Hierarchy", true, ImVec2(300.0f, 200.0f), &Docks::render_hierarchy);

	auto& inspector = _docks[3];
	inspector->initialize("Inspector", true, ImVec2(300.0f, 200.0f), &Docks::render_inspector);

	auto& assets = _docks[4];
	assets->initialize("Assets", true, ImVec2(200.0f, 200.0f), &Docks::render_assets);

	auto& console = _docks[5];
	console->initialize("Console", true, ImVec2(200.0f, 200.0f), [this](ImVec2 area)
	{
		Docks::render_console(area, *_console_log.get());
	});

	auto& style = _docks[6];
	style->initialize("Style", true, ImVec2(300.0f, 200.0f), &Docks::render_style);

	auto engine = core::get_subsystem<runtime::Engine>();
	const auto& windows = engine->get_windows();
	auto& window = static_cast<GuiWindow&>(*windows[0]);
	auto& dockspace = window.get_dockspace();
	dockspace.dock(scene.get(), ImGuiDock::DockSlot::None, 200, true);
	dockspace.dock_with(game.get(), scene.get(), ImGuiDock::DockSlot::Tab, 300, false);
	dockspace.dock_with(inspector.get(), scene.get(), ImGuiDock::DockSlot::Right, 300, true);
	dockspace.dock_with(hierarchy.get(), scene.get(), ImGuiDock::DockSlot::Left, 300, true);
	dockspace.dock(assets.get(), ImGuiDock::DockSlot::Bottom, 250, true);
	dockspace.dock_with(console.get(), assets.get(), ImGuiDock::DockSlot::Tab, 250, true);
	dockspace.dock_with(style.get(), assets.get(), ImGuiDock::DockSlot::Right, 300, true);

	auto logger = logging::get("Log");
	logger->add_sink(_console_log);

	std::function<void()> logVersion = [logger]()
	{
		logger->info() << "Version 1.0";
	};
	_console_log->register_command(
		"version",
		"Returns the current version of the Editor.",
		{},
		{},
		logVersion
	);

	return true;
}
