#include "docking.h"

bool docking_system::initialize()
{
	runtime::on_platform_events.connect(this, &docking_system::platform_events);

	return true;
}

void docking_system::dispose()
{
	runtime::on_platform_events.disconnect(this, &docking_system::platform_events);

	_docks.clear();
	_dockspaces.clear();
}

imguidock::dockspace& docking_system::get_dockspace(uint32_t id)
{
	auto it = _dockspaces.find(id);
	if(it == _dockspaces.end())
	{
		imguidock::dockspace dockspace;
		dockspace.owner_id = id;
		_dockspaces.emplace(id, std::move(dockspace));
	}

	return _dockspaces[id];
}

const std::map<uint32_t, imguidock::dockspace>& docking_system::get_dockspaces() const
{
	return _dockspaces;
}

void docking_system::register_dock(std::unique_ptr<imguidock::dock> dock)
{
	_docks.emplace_back(std::move(dock));
}

void docking_system::platform_events(const std::pair<std::uint32_t, bool>& info,
									 const std::vector<mml::platform_event>& events)
{
    const auto window_id = info.first;
	for(const auto& e : events)
	{
		if(e.type == mml::platform_event::closed)
		{
			_dockspaces.erase(window_id);
			return;
		}
	}
}
