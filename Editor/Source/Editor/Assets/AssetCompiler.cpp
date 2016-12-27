#include "AssetCompiler.h"
#include "Core/common/string_utils.h"
#include "Core/logging/logging.h"
#include "Runtime/System/FileSystem.h"
#include "ShaderCompiler/shaderc.h"

void ShaderCompiler::compile(const fs::path& absoluteKey)
{
	fs::path input = absoluteKey;
	std::string strInput = input.string();
	std::string file = input.filename().replace_extension().string();
	fs::path dir = input.remove_filename();

	static const std::string ext = ".asset";

	bool vs = string_utils::beginsWith(file, "vs_");
	bool fs = string_utils::beginsWith(file, "fs_");
	bool cs = string_utils::beginsWith(file, "cs_");
	fs::path supported[] = { "dx9", "dx11", "glsl", "metal" };
	
	for (int i = 0; i < 4; ++i)
	{
		fs::path output = dir / "runtime";
		fs::create_directory(output, std::error_code{});

		output = output / supported[i] / fs::path(file + ext);
		fs::create_directory(output, std::error_code{});
		std::string strOutput = output.string();

		const char* args_array[18];
		args_array[0] = "-f";
		args_array[1] = strInput.c_str();
		args_array[2] = "-o";
		args_array[3] = strOutput.c_str();
		args_array[4] = "--depends";
		args_array[5] = "-i";
		fs::path include = fs::resolve_protocol("engine://Tools/include");
		std::string strInclude = include.string();
		args_array[6] = strInclude.c_str();
		args_array[7] = "--varyingdef";
		fs::path varying = dir / "varying.def.sc";
		std::string strVarying = varying.string();
		args_array[8] = strVarying.c_str();
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
		
		
		if (i >= 2)
		{
			//glsl shader compilation is not thread safe-
			static std::mutex mtx;
			std::lock_guard<std::mutex> lock(mtx);
			if (compileShader(18, args_array) == EXIT_FAILURE)
			{
				logger->error().write("Failed to compile shader: {0}", strOutput.c_str());
			}
			else
			{
				logger->info().write("Successfully compiled shader: {0}", strOutput.c_str());
			}
		}
		else
		{
			if (compileShader(18, args_array) == EXIT_FAILURE)
			{
				logger->error().write("Failed to compile shader: {0}", strOutput.c_str());
			}
			else
			{
				logger->info().write("Successfully compiled shader: {0}", strOutput.c_str());
			}
		}
		
	}
	
}
