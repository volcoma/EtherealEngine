#pragma once

#include "core/subsystem/subsystem.h"
#include <vector>
#include <memory>
#include "ImGuiDock.h"

struct DockingSystem : public core::Subsystem
{
	bool initialize();
	void dispose();
private:
	std::vector<std::unique_ptr<ImGuiDock::Dock>> _docks;
};