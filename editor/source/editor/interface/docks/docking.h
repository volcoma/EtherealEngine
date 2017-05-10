#pragma once

#include "core/subsystem/subsystem.h"
#include <vector>
#include <memory>
#include "ImGuiDock.h"

struct docking_system : public core::subsystem
{
	bool initialize();
	void dispose();
private:
	std::vector<std::unique_ptr<imguidock::dock>> _docks;
};