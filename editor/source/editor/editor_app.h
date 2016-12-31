#pragma once

#include "runtime/system/app.h"
#include "runtime/system/singleton.h"

namespace editor
{
	struct EditorApp : public runtime::App
	{
		// setup before engine initialization. this is a chance to eg. modify the engine parameters.
		virtual void setup() {}
		// setup after engine initialization and before running the main loop
		virtual void start();
		// cleanup after the main loop
		virtual void stop() {}
	};

}

template<>
inline runtime::App& Singleton<runtime::App>::create()
{
	static editor::EditorApp app;
	return app;
};

template<>
inline editor::EditorApp& Singleton<editor::EditorApp>::create()
{
	return static_cast<editor::EditorApp&>(Singleton<runtime::App>::getInstance());
};