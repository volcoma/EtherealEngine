#include "system/app.h"
#include "runtime/runtime.h"

int main(int _argc, char* _argv[])
{
	//fs::path exe_path = fs::canonical(fs::executable_path(_argv[0]).remove_filename());
	fs::path engine_path = fs::system_complete(ENGINE_DIRECTORY);
	fs::path engine_data = engine_path / "engine_data";
	fs::path editor_data = engine_path / "editor_data";
	fs::add_path_protocol("engine:", engine_path.string());
	fs::add_path_protocol("engine_data:", engine_data.string());
	fs::add_path_protocol("editor_data:", editor_data.string());

	editor::app app;
	int return_code = app.run();

	return return_code;
}
