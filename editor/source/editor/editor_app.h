#pragma once

#include "runtime/system/app.h"
#include "runtime/system/singleton.h"

namespace editor
{
	struct EditorApp : public runtime::App
	{
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
	};

}

template<>
inline runtime::App& singleton<runtime::App>::create()
{
	static editor::EditorApp app;
	return app;
};

template<>
inline editor::EditorApp& singleton<editor::EditorApp>::create()
{
	return static_cast<editor::EditorApp&>(singleton<runtime::App>::get_instance());
};