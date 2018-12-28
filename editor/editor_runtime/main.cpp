#include "meta/meta.h"
#include "system/app.h"

#include <core/filesystem/filesystem.h>


int main(int argc, char* argv[])
{
	fs::path engine_path = fs::system_complete(ENGINE_DIRECTORY);
	fs::path shader_include_path = fs::system_complete(SHADER_INCLUDE_DIRECTORY);

	fs::path engine = engine_path / "engine_data";
	fs::path editor = engine_path / "editor_data";
	fs::path binary_path = fs::executable_path(argv[0]).parent_path();
	fs::add_path_protocol("engine:", engine);
	fs::add_path_protocol("editor:", editor);
	fs::add_path_protocol("binary:", binary_path);
	fs::add_path_protocol("shader_include:", shader_include_path);
	editor::app app;
	int return_code = app.run(argc, argv);

	return return_code;
}
