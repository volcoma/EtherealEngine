#include "AssetCompiler.h"
#include "Core/common/string_utils.h"
#include "Core/logging/logging.h"
#include "Runtime/System/FileSystem.h"
#include "ShaderCompiler/shaderc.h"

void ShaderCompiler::compile(const std::string& absoluteKey)
{
	std::string input = fs::resolveFileLocation(absoluteKey);
	input = string_utils::replace(input, '\\', '/');
	input = string_utils::toLower(input);
	std::string dir = fs::getDirectoryName(input);
	
	std::string file = fs::getFileName(input, true);
	dir = string_utils::replace(dir, '\\', '/');
	dir = string_utils::toLower(dir);
	file = string_utils::toLower(file);
	static const std::string ext = ".asset";

	bool vs = string_utils::beginsWith(file, "vs_");
	bool fs = string_utils::beginsWith(file, "fs_");
	bool cs = string_utils::beginsWith(file, "cs_");
	const char* supported[] = { "dx9", "dx11", "glsl", "metal" };
	
	for (int i = 0; i < 4; ++i)
	{
		std::string output = dir + "/runtime/";
		fs::ensurePath(output, false);

		output = output + std::string(supported[i]) + "/" + file + ext;
		fs::ensurePath(output, false);

		const char* args_array[18];
		args_array[0] = "-f";
		args_array[1] = input.c_str();
		args_array[2] = "-o";
		args_array[3] = output.c_str();
		args_array[4] = "--depends";
		args_array[5] = "-i";
		std::string include = fs::resolveFileLocation("engine://Tools/include/");
		include = string_utils::replace(include, '\\', '/');
		include = string_utils::toLower(include);
		args_array[6] = include.c_str();
		args_array[7] = "--varyingdef";
		std::string varying = dir + "/varying.def.sc";
		args_array[8] = varying.c_str();
		args_array[9] = "--platform";

		if(i < 2)
		{
			args_array[10] = "windows";
			args_array[11] = "--profile";

			if (vs)
				args_array[12] = "vs_4_0";
			else if (fs)
				args_array[12] = "ps_4_0";
			else if (cs)
				args_array[12] = "cs_5_0";
		}
		else if (i == 2)
		{
			args_array[10] = "linux";
			args_array[11] = "--profile";

			if (vs || fs)
				args_array[12] = "120";
			else if (cs)
				args_array[12] = "430";
		}
		else if (i == 3)
		{
			args_array[10] = "osx";
			args_array[11] = "--profile";
			args_array[12] = "metal";
		}
		args_array[13] = "--type";
		if (vs)
			args_array[14] = "vertex";
		else if (fs)
			args_array[14] = "fragment";
		else if (cs)
			args_array[14] = "compute";

		args_array[15] = "-O";
		args_array[16] = "3";
		args_array[17] = "--disasm";

		auto logger = logging::get("Log");
		static std::mutex mtx;
		std::lock_guard<std::mutex> lock(mtx);
		if (compileShader(18, args_array) == EXIT_FAILURE)
		{
			logger->error().write("Failed to compile shader: {0}", output.c_str());
		}
		else
		{
			logger->info().write("Successfully compiled shader: {0}", output.c_str());
		}
	}
	
}
