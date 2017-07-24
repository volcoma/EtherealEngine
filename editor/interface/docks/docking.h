#pragma once

#include "core/system/subsystem.h"
#include "imguidock.h"
#include <memory>
#include <vector>

struct docking_system : public core::subsystem
{
	bool initialize();
	void dispose();

private:
	std::vector<std::unique_ptr<imguidock::dock>> _docks;
};
