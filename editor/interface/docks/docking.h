#pragma once

#include "core/system/subsystem.h"
#include "imguidock.h"
#include "runtime/system/events.h"
#include <map>
#include <memory>
#include <vector>

struct docking_system : public core::subsystem
{
	bool initialize();
	void dispose();

	imguidock::dockspace& get_dockspace(std::uint32_t id);
	const std::map<uint32_t, imguidock::dockspace>& get_dockspaces() const;
	void register_dock(std::unique_ptr<imguidock::dock> dock);

private:
	void platform_events(const std::pair<std::uint32_t, bool>& info,
						 const std::vector<mml::platform_event>& events);

	std::map<uint32_t, imguidock::dockspace> _dockspaces;
	std::vector<std::unique_ptr<imguidock::dock>> _docks;
};
