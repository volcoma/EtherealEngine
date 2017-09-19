#include "docking.h"
#include "../../console/console_log.h"
#include "../../system/editor_window.h"
#include "assets_dock.h"
#include "console_dock.h"
#include "core/logging/logging.h"
#include "game_dock.h"
#include "hierarchy_dock.h"
#include "inspector_dock.h"
#include "runtime/system/engine.h"
#include "scene_dock.h"
#include "style_dock.h"

bool docking_system::initialize()
{
	auto log = std::make_shared<console_log>();
	_docks.emplace_back(std::make_unique<scene_dock>("SCENE", true, ImVec2(200.0f, 200.0f)));
	auto scene = _docks.back().get();
	_docks.emplace_back(std::make_unique<game_dock>("GAME", true, ImVec2(200.0f, 200.0f)));
	auto game = _docks.back().get();
	_docks.emplace_back(std::make_unique<hierarchy_dock>("HIERARCHY", true, ImVec2(300.0f, 200.0f)));
	auto hierarchy = _docks.back().get();
	_docks.emplace_back(std::make_unique<inspector_dock>("INSPECTOR", true, ImVec2(300.0f, 200.0f)));
	auto inspector = _docks.back().get();
	_docks.emplace_back(std::make_unique<assets_dock>("ASSETS", true, ImVec2(200.0f, 200.0f)));
	auto assets = _docks.back().get();
	_docks.emplace_back(std::make_unique<console_dock>("CONSOLE", true, ImVec2(200.0f, 200.0f), log));
	auto console = _docks.back().get();
	_docks.emplace_back(std::make_unique<style_dock>("STYLE", true, ImVec2(300.0f, 200.0f)));
	auto style = _docks.back().get();

	auto& engine = core::get_subsystem<runtime::engine>();
	const auto& windows = engine.get_windows();
	auto& window = static_cast<main_editor_window&>(*windows.front());
	auto& dockspace = window.get_dockspace();
	dockspace.dock_to(scene, imguidock::slot::tab, 200, true);
	dockspace.dock_with(game, scene, imguidock::slot::tab, 300, false);
	dockspace.dock_with(inspector, scene, imguidock::slot::right, 400, true);
	dockspace.dock_with(hierarchy, scene, imguidock::slot::left, 300, true);
	dockspace.dock_to(console, imguidock::slot::bottom, 300, true);
	dockspace.dock_with(assets, console, imguidock::slot::tab, 250, true);
	dockspace.dock_with(style, assets, imguidock::slot::right, 400, true);

	auto logging_container = logging::get_mutable_logging_container();
	logging_container->add_sink(log);
	window.set_log("CONSOLE", log);
	std::function<void()> log_version = []() { APPLOG_INFO("Version 1.0"); };
	log->register_command("version", "Returns the current version of the Editor.", {}, {}, log_version);

	return true;
}

void docking_system::dispose()
{
	_docks.clear();
}
