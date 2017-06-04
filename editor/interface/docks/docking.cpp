#include "docking.h"
#include "runtime/system/engine.h"
#include "../../system/editor_window.h"
#include "core/logging/logging.h"
#include "scene_dock.h"
#include "game_dock.h"
#include "hierarchy_dock.h"
#include "inspector_dock.h"
#include "assets_dock.h"
#include "console_dock.h"
#include "style_dock.h"
#include "../../console/console_log.h"

bool docking_system::initialize()
{
	auto log = std::make_shared<console_log>();
	_docks.emplace_back(std::make_unique<scene_dock>("Scene", true, ImVec2(200.0f, 200.0f)));
	_docks.emplace_back(std::make_unique<game_dock>("Game", true, ImVec2(200.0f, 200.0f)));
	_docks.emplace_back(std::make_unique<hierarchy_dock>("Hierarchy", true, ImVec2(300.0f, 200.0f)));
	_docks.emplace_back(std::make_unique<inspector_dock>("Inspector", true, ImVec2(300.0f, 200.0f)));
	_docks.emplace_back(std::make_unique<assets_dock>("Assets", true, ImVec2(200.0f, 200.0f)));
	_docks.emplace_back(std::make_unique<console_dock>("Console", true, ImVec2(200.0f, 200.0f), log));
	_docks.emplace_back(std::make_unique<style_dock>("Style", true, ImVec2(300.0f, 200.0f)));

	auto& scene = _docks[0];
	auto& game = _docks[1];
	auto& hierarchy = _docks[2];
	auto& inspector = _docks[3];
	auto& assets = _docks[4];
	auto& console = _docks[5];
	auto& style = _docks[6];

	auto& engine = core::get_subsystem<runtime::engine>();
	const auto& windows = engine.get_windows();
	auto& window = static_cast<main_editor_window&>(*windows[0]);
	auto& dockspace = window.get_dockspace();
	dockspace.dock_to(scene.get(), imguidock::slot::none, 200, true);
	dockspace.dock_with(game.get(), scene.get(), imguidock::slot::tab, 300, false);
	dockspace.dock_with(inspector.get(), scene.get(), imguidock::slot::right, 300, true);
	dockspace.dock_with(hierarchy.get(), scene.get(), imguidock::slot::left, 300, true);
	dockspace.dock_to(console.get(), imguidock::slot::bottom, 250, true);
	dockspace.dock_with(assets.get(), console.get(), imguidock::slot::tab, 250, true);
	dockspace.dock_with(style.get(), assets.get(), imguidock::slot::right, 300, true);

	auto logger = logging::get(APPLOG);
	logger->add_sink(log);
	window.set_log("Console", log);
	std::function<void()> log_version = []()
	{
		APPLOG_INFO("Version 1.0");
	};
	log->register_command(
		"version",
		"Returns the current version of the Editor.",
		{},
		{},
		log_version
	);

	return true;
}

void docking_system::dispose()
{
	_docks.clear();
}
