#pragma once

#include "core/subsystem.h"

namespace runtime
{
	struct App
	{
		App();

		//-----------------------------------------------------------------------------
		//  Name : setup (virtual )
		/// <summary>
		/// Setup before engine initialization. this is a chance to eg. modify 
		/// the engine parameters.
		/// </summary>
		//-----------------------------------------------------------------------------

		virtual void setup() {}
		//-----------------------------------------------------------------------------
		//  Name : start (virtual )
		/// <summary>
		///  setup after engine initialization and before running the main loop
		/// </summary>
		//-----------------------------------------------------------------------------
		virtual void start();

		//-----------------------------------------------------------------------------
		//  Name : stop (virtual )
		/// <summary>
		/// Cleanup after the main loop.
		/// </summary>
		//-----------------------------------------------------------------------------
		virtual void stop() {}

		//-----------------------------------------------------------------------------
		//  Name : run ()
		/// <summary>
		/// Initialize the engine and run the main loop, then return the
		/// application exit code.
		/// </summary>
		//-----------------------------------------------------------------------------
		int run();

		//-----------------------------------------------------------------------------
		//  Name : terminate_with_error ()
		/// <summary>
		/// Show  an error message, terminate the main loop, and set failure exit code.
		/// </summary>
		//-----------------------------------------------------------------------------
		void terminate_with_error(const std::string&);

	protected:
		/// exit code of the application
		int _exitcode;
	};

}