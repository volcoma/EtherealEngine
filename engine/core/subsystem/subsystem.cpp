#include "subsystem.h"
#include <memory>

namespace core
{

	bool subsystem_context::initialize()
	{
		return true;
	}

	void subsystem_context::dispose()
	{
		for (auto iter = _orders.rbegin(); iter != _orders.rend(); iter++)
		{
			auto found = _subsystems.find(*iter);
			ensures(found != _subsystems.end());

			found->second->dispose();
			found->second.reset();
			_subsystems.erase(found);
		}

		_orders.clear();
		ensures(_subsystems.size() == 0);
	}

	namespace details
	{
		internal_status& status()
		{
			static internal_status s_status = internal_status::idle;
			return s_status;
		}

		void dispose()
		{
			context().dispose();
			context() = {};
			status() = internal_status::disposed;
		}

		subsystem_context& context()
		{
			static subsystem_context s_context;
			return s_context;
		}

		bool initialize()
		{
			if (!context().initialize())
				return false;

			status() = internal_status::running;
			return true;
		}
	}
}
