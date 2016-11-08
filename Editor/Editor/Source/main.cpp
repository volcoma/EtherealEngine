#include "EditorApp.h"
#include "Runtime/runtime.h"

int main(int _argc, char* _argv[])
{
	auto& app = Singleton<Application>::getInstance();

	auto rootDataDir = fs::getAppDirectory() + "../../";
	if (!app.initInstance(rootDataDir, string_utils::commandLineArgs(_argc, _argv)))
	{
		// Release the framework
		return -1;
	}

	int returnCode = app.begin();

	// Shut down the application, just to be polite, before exiting.
	app.shutDown();

	return returnCode;
}