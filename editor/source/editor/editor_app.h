#pragma once

#include "runtime/system/app.h"
#include "runtime/system/singleton.h"

namespace editor
{
	struct EditorApp : public runtime::App
	{
		//-----------------------------------------------------------------------------
		//  Name : start (virtual )
		/// <summary>
		///  setup after engine initialization and before running the main loop
		/// </summary>
		//-----------------------------------------------------------------------------
		virtual void start();
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