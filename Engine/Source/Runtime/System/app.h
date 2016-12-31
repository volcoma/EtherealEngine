#pragma once

#include "core/subsystem.h"

namespace runtime
{
	struct App
	{
		App();

		// setup before engine initialization. this is a chance to eg. modify the engine parameters.
		virtual void setup() {}
		// setup after engine initialization and before running the main loop
		virtual void start();
		// cleanup after the main loop
		virtual void stop() {}

		// initialize the engine and run the main loop, then return the
		// application exit code
		int run();
		// show  an error message, terminate the main loop, and set failure exit code
		void terminate_with_error(const std::string&);

	protected:
		int _exitcode;
	};

}