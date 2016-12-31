#include "subsystem.h"
#include <memory>

namespace core
{

	bool SubsystemContext::initialize()
	{
		return true;
	}

	void SubsystemContext::dispose()
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
		std::unique_ptr<SubsystemContext> s_context;
		Status s_status = Status::IDLE;

		bool initialize()
		{
			auto context = std::unique_ptr<SubsystemContext>(new (std::nothrow) SubsystemContext());
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

		SubsystemContext& context()
		{
			return *s_context;
		}
	}
}
