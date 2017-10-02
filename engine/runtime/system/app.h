#pragma once

#include "core/system/subsystem.h"

namespace runtime
{
struct app
{
	virtual ~app() = default;
	//-----------------------------------------------------------------------------
	//  Name : setup (virtual )
	/// <summary>
	/// Setup before engine initialization. this is a chance to eg. modify
	/// the engine parameters.
	/// </summary>
	//-----------------------------------------------------------------------------

	virtual void setup();
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
	virtual void stop();
    
    virtual void run_one_frame();
    
	//-----------------------------------------------------------------------------
	//  Name : run ()
	/// <summary>
	/// Initialize the engine and run the main loop, then return the
	/// application exit code.
	/// </summary>
	//-----------------------------------------------------------------------------
	int run();

	//-----------------------------------------------------------------------------
	//  Name : quit_with_error ()
	/// <summary>
	/// Show an error message, terminate the main loop, and set failure exit code.
	/// </summary>
	//-----------------------------------------------------------------------------
	void quit_with_error(const std::string&);

	//-----------------------------------------------------------------------------
	//  Name : quit ()
	/// <summary>
	/// Terminate the main loop and exit the app.
	/// </summary>
	//-----------------------------------------------------------------------------
	void quit(int exitcode = 0);

protected:
	/// exit code of the application
	int _exitcode = 0;
    bool _running = true;
};
}
