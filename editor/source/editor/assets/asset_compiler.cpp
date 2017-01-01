#include "asset_compiler.h"
#include "core/common/string.h"
#include "core/logging/logging.h"
#include "runtime/system/FileSystem.h"
#include "shaderc/shaderc.h"

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

		output = output / supported[i];
		fs::create_directory(output, std::error_code{});
		output /= fs::path(file + ext);

		std::string strOutput = output.string();

		static const int arg_count = 16;
		const char* args_array[arg_count];
		args_array[0] = "-f";
		args_array[1] = strInput.c_str();
		args_array[2] = "-o";
		args_array[3] = strOutput.c_str();
		args_array[4] = "-i";
		fs::path include = fs::resolve_protocol("engine://Tools/include");
		std::string strInclude = include.string();
		args_array[5] = strInclude.c_str();
		args_array[6] = "--varyingdef";
		fs::path varying = dir / "varying.def.sc";
		std::string strVarying = varying.string();
		args_array[7] = strVarying.c_str();
		args_array[8] = "--platform";

		if(i < 2)
		{
			args_array[9] = "windows";
			args_array[10] = "--profile";

			if (vs)
				args_array[11] = "vs_4_0";
			else if (fs)
				args_array[11] = "ps_4_0";
			else if (cs)
				args_array[11] = "cs_5_0";
		}
		else if (i == 2)
		{
			args_array[9] = "linux";
			args_array[10] = "--profile";

			if (vs || fs)
				args_array[11] = "120";
			else if (cs)
				args_array[11] = "430";
		}
		else if (i == 3)
		{
			args_array[9] = "osx";
			args_array[10] = "--profile";
			args_array[11] = "metal";
		}
		args_array[12] = "--type";
		if (vs)
			args_array[13] = "vertex";
		else if (fs)
			args_array[13] = "fragment";
		else if (cs)
			args_array[13] = "compute";

		args_array[14] = "-O";
		args_array[15] = "3";

		auto logger = logging::get("Log");
		
		
		if (i >= 2)
		{
			//glsl shader compilation is not thread safe-
			static std::mutex mtx;
			std::lock_guard<std::mutex> lock(mtx);
			if (compileShader(arg_count, args_array) == EXIT_FAILURE)
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
			if (compileShader(arg_count, args_array) == EXIT_FAILURE)
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
