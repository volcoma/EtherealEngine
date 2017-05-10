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
			Ensures(found != _subsystems.end());

			found->second->dispose();
			delete found->second;
			_subsystems.erase(found);
		}

		_orders.clear();
		Ensures(_subsystems.size() == 0);
	}

	namespace details
	{
		std::unique_ptr<subsystem_context> s_context;
		Status s_status = Status::IDLE;

		bool initialize()
		{
			auto context = std::unique_ptr<subsystem_context>(new (std::nothrow) subsystem_context());
			if (context.get() == nullptr || !context->initialize())
				return false;

			s_context = std::move(context);
			s_status = Status::RUNNING;
			return true;
		}

		Status status()
		{
			return s_status;
		}

		void dispose()
		{
			if (s_context.get())
			{
				s_context->dispose();
				s_context.reset();
			}
			s_status = Status::DISPOSED;
		}

		subsystem_context& context()
		{
			return *s_context;
		}
	}
}
