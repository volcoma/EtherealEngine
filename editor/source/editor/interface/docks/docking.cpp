#include "docking.h"
#include "runtime/system/engine.h"
#include "../gui_window.h"
#include "core/logging/logging.h"
#include "scene_dock.h"
#include "game_dock.h"
#include "hierarchy_dock.h"
#include "inspector_dock.h"
#include "assets_dock.h"
#include "console_dock.h"
#include "style_dock.h"
#include "../../console/console_log.h"

bool DockingSystem::initialize()
{
	std::shared_ptr<ConsoleLog> console_log = std::make_shared<ConsoleLog>();
	_docks.emplace_back(std::make_unique<SceneDock>("Scene", true, ImVec2(200.0f, 200.0f)));
	_docks.emplace_back(std::make_unique<GameDock>("Game", true, ImVec2(200.0f, 200.0f)));
	_docks.emplace_back(std::make_unique<HierarchyDock>("Hierarchy", true, ImVec2(300.0f, 200.0f)));
	_docks.emplace_back(std::make_unique<InspectorDock>("Inspector", true, ImVec2(300.0f, 200.0f)));
	_docks.emplace_back(std::make_unique<AssetsDock>("Assets", true, ImVec2(200.0f, 200.0f)));
	_docks.emplace_back(std::make_unique<ConsoleDock>("Console", true, ImVec2(200.0f, 200.0f), console_log));
	_docks.emplace_back(std::make_unique<StyleDock>("Style", true, ImVec2(300.0f, 200.0f)));

	auto& scene = _docks[0];
	auto& game = _docks[1];
	auto& hierarchy = _docks[2];
	auto& inspector = _docks[3];
	auto& assets = _docks[4];
	auto& console = _docks[5];
	auto& style = _docks[6];

	auto engine = core::get_subsystem<runtime::Engine>();
	const auto& windows = engine->get_windows();
	auto& window = static_cast<GuiWindow&>(*windows[0]);
	auto& dockspace = window.get_dockspace();
	dockspace.dock(scene.get(), ImGuiDock::DockSlot::None, 200, true);
	dockspace.dock_with(game.get(), scene.get(), ImGuiDock::DockSlot::Tab, 300, false);
	dockspace.dock_with(inspector.get(), scene.get(), ImGuiDock::DockSlot::Right, 300, true);
	dockspace.dock_with(hierarchy.get(), scene.get(), ImGuiDock::DockSlot::Left, 300, true);
	dockspace.dock(console.get(), ImGuiDock::DockSlot::Bottom, 250, true);
	dockspace.dock_with(assets.get(), console.get(), ImGuiDock::DockSlot::Tab, 250, true);
	dockspace.dock_with(style.get(), assets.get(), ImGuiDock::DockSlot::Right, 300, true);

	auto logger = logging::get(APPLOG);
	logger->add_sink(console_log);

	std::function<void()> log_version = []()
	{
		APPLOG_INFO("Version 1.0");
	};
	console_log->register_command(
		"version",
		"Returns the current version of the Editor.",
		{},
		{},
		log_version
	);

	return true;
}

void DockingSystem::dispose()
{
	_docks.clear();
}
