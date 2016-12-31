#include "runtime/runtime.h"
#include "editor_app.h"
//Regex to count lines of code
//^(?!(\s*\*))(?!(\s*\-\-\>))(?!(\s*\<\!\-\-))(?!(\s*\n))(?!(\s*\*\/))(?!(\s*\/\*))(?!(\s*\/\/\/))(?!(\s*\/\/))(?!(\s*\}))(?!(\s*\{))(?!(\s(using))).*$


int main(int _argc, char* _argv[])
{
	fs::path exe_path = fs::canonical(fs::executable_path(_argv[0]).remove_filename());
	fs::path engine_path = fs::canonical(fs::path("../../../../"), exe_path);
	fs::path engine_data = fs::canonical(fs::path("../../../../engine_data/"), exe_path);
	fs::path editor_data = fs::canonical(fs::path("../../../../editor_data/"), exe_path);
	fs::add_path_protocol("engine:", engine_path.string());
	fs::add_path_protocol("engine_data:", engine_data.string());
	fs::add_path_protocol("editor_data:", editor_data.string());

	auto& app = Singleton<runtime::App>::getInstance();
	int returnCode = app.run();
// 	
// 	auto& app = Singleton<Application>::getInstance();
// 	if (!app.initInstance(string_utils::commandLineArgs(_argc, _argv)))
// 	{
// 		// Release the framework
// 		app.shutDown();
// 		return -1;
// 	}
// 
// 	int returnCode = app.begin();
// 
// 	// Shut down the application, just to be polite, before exiting.
// 	app.shutDown();

	return returnCode;
}