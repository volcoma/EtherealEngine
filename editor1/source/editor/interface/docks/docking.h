#pragma once

#include "core/subsystem.h"
#include <vector>
#include <memory>
#include "ImGuiDock.h"
#include "../../console/console_log.h"

struct DockingSystem : public core::Subsystem
{
	bool initialize();

private:
	std::vector<std::unique_ptr<ImGuiDock::Dock>> _docks;
	std::shared_ptr<ConsoleLog> _console_log = std::make_shared<ConsoleLog>();
};