#include "asset_compiler.h"
#include "bx/error.h"
#include "bx/process.h"
#include "bx/string.h"
#include "core/filesystem/filesystem.h"
#include "core/graphics/graphics.h"
#include "core/logging/logging.h"
#include "core/serialization/binary_archive.h"
#include "core/serialization/serialization.h"
#include "core/serialization/types/unordered_map.hpp"
#include "core/serialization/types/vector.hpp"
#include "core/string_utils/string_utils.h"
#include "core/uuid/uuid.hpp"
#include "mesh_importer.h"
#include "runtime/assets/asset_extensions.h"
#include "runtime/meta/animation/animation.hpp"
#include "runtime/meta/rendering/mesh.hpp"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/shader.h"
#include "runtime/rendering/texture.h"
#include <array>
#include <fstream>

class material;
struct prefab;
struct scene;

namespace asset_compiler
{
std::string escape_str(const std::string& str)
{
	return "\"" + str + "\"";
}

bool run_compile_process(const std::string& process, const std::vector<std::string>& args_array,
						 std::string& err)
{

	std::string args;
	size_t i = 0;
	for(const auto& arg : args_array)
	{
		if(arg.front() == '-')
		{
			args += arg;
		}
		else
		{
			args += escape_str(arg);
		}

		if(i++ != args_array.size() - 1)
			args += " ";
	}

	bx::Error error;
	bx::ProcessReader process_reader;

	auto executable_dir = fs::resolve_protocol("binary:/");
	auto process_full = executable_dir / process;
#if($on($windows))
	process_reader.open((process_full.string() + " " + args).c_str(), "", &error);
#else
    process_reader.open(process_full.string().c_str(), args.c_str(), &error);    
#endif
	if(!error.isOk())
	{
		err = std::string(error.getMessage().getPtr());
		return false;
	}
	else
	{
		char buffer[2048];
		process_reader.read(buffer, sizeof(buffer), &error);

		process_reader.close();
		int32_t result = process_reader.getExitCode();

		if(0 != result)
		{
			err = std::string(error.getMessage().getPtr());
			return false;
		}

		return true;
	}
}

template <>
void compile<prefab>(const fs::path&)
{
}

template <>
void compile<scene>(const fs::path&)
{
}

template <>
void compile<material>(const fs::path&)
{
}

template <>
void compile<shader>(const fs::path& absolute_key)
{
	fs::path output = absolute_key.string() + extensions::get_compiled_format<shader>();
	std::string str_input = absolute_key.string();
	std::string file = absolute_key.stem().string();
	fs::path dir = absolute_key.parent_path();

	fs::error_code err;
	fs::path temp = fs::temp_directory_path(err);
	temp /= uuids::random_uuid(str_input).to_string() + ".buildtemp";

	std::string str_output = temp.string();
	fs::path include = fs::resolve_protocol("shader_include:/");
	std::string str_include = include.string();
	fs::path varying = dir / (file + ".io");
	std::string str_varying = varying.string();

	std::string str_platform;
	std::string str_profile;
	std::string str_type;

	bool vs = string_utils::begins_with(file, "vs_");
	bool fs = string_utils::begins_with(file, "fs_");
	bool cs = string_utils::begins_with(file, "cs_");

	auto renderer = gfx::getRendererType();
	if(renderer == gfx::RendererType::Direct3D11 || renderer == gfx::RendererType::Direct3D12)
	{
		str_platform = "windows";

		if(vs)
			str_profile = "vs_4_0";
		else if(fs)
			str_profile = "ps_4_0";
		else if(cs)
			str_profile = "cs_5_0";
	}
	else if(renderer == gfx::RendererType::OpenGL)
	{
		str_platform = "linux";

		if(vs || fs)
			str_profile = "120";
		else if(cs)
			str_profile = "430";
	}
	else if(renderer == gfx::RendererType::Metal)
	{
		str_platform = "osx";
		str_profile = "metal";
	}

	if(vs)
		str_type = "vertex";
	else if(fs)
		str_type = "fragment";
	else if(cs)
		str_type = "compute";
	else
		str_type = "unknown";

	const std::vector<std::string> args_array = {
		"-f",		  str_input.c_str(),	"-o",			str_output.c_str(),
		"-i",		  str_include.c_str(),  "--varyingdef", str_varying.c_str(),
		"--platform", str_platform.c_str(), "-p",			str_profile.c_str(),
		"--type",	 str_type.c_str(),		"-O",			"3",
	};

	std::string error;

	{
		std::ofstream output_file(str_output);
	}

	if(!run_compile_process("shaderc", args_array, error))
	{
		APPLOG_ERROR("Failed compilation of {0} with error: {1}", str_input, error);
	}
	else
	{
		APPLOG_INFO("Successful compilation of {0}", str_input);
		fs::copy_file(temp, output, fs::copy_option::overwrite_if_exists, err);
	}
	fs::remove(temp, err);
}

template <>
void compile<texture>(const fs::path& absolute_key)
{
	fs::path output = absolute_key.string() + extensions::get_compiled_format<texture>();
	std::string str_input = absolute_key.string();

	fs::error_code err;
	fs::path temp = fs::temp_directory_path(err);
    temp /= uuids::random_uuid(str_input).to_string() + ".buildtemp";
    
	std::string str_output = temp.string();

	const std::vector<std::string> args_array = {
		"-f", str_input.c_str(), "-o", str_output.c_str(), "--as", "ktx", "-m", "-t", "BGRA8",
	};

	std::string error;

	{
		std::ofstream output_file(str_output);
	}

	if(!run_compile_process("texturec", args_array, error))
	{
		APPLOG_ERROR("Failed compilation of {0} with error: {1}", str_input, error);
	}
	else
	{
		APPLOG_INFO("Successful compilation of {0}", str_input);
		fs::copy_file(temp, output, fs::copy_option::overwrite_if_exists, err);
	}
	fs::remove(temp, err);
}

template <>
void compile<mesh>(const fs::path& absolute_key)
{
	fs::path output = absolute_key.string() + extensions::get_compiled_format<mesh>();
	std::string str_input = absolute_key.string();

	fs::error_code err;
	fs::path temp = fs::temp_directory_path(err);
    temp /= uuids::random_uuid(str_input).to_string() + ".buildtemp";

	mesh::load_data data;
	std::vector<animation> animations;
	if(!importer::load_mesh_data_from_file(str_input, data, animations))
	{
		APPLOG_ERROR("Failed compilation of {0}", str_input);
		return;
	}

	if(data.vertex_data.empty() == false)
	{
		{
			std::ofstream soutput(temp.string(), std::ios::out | std::ios::binary);
			cereal::oarchive_binary_t ar(soutput);
			try_save(ar, cereal::make_nvp("mesh", data));
		}
		fs::copy_file(temp, output, fs::copy_option::overwrite_if_exists, err);
		fs::remove(temp, err);

		APPLOG_INFO("Successful compilation of {0}", str_input);
		{
			fs::path file = absolute_key.stem();
			fs::path dir = absolute_key.parent_path();

			for(const auto& animation : animations)
			{
				temp = fs::temp_directory_path(err);
				temp.append(uuids::random_uuid(str_input).to_string() + ".buildtemp");
				{
					std::ofstream soutput(temp.string(), std::ios::out | std::ios::binary);
					cereal::oarchive_binary_t ar(soutput);
					try_save(ar, cereal::make_nvp("animation", animation));
				}
				output = (dir / file).string() + "_" + animation.name + extensions::animation;

				fs::copy_file(temp, output, fs::copy_option::overwrite_if_exists, err);
				fs::remove(temp, err);
			}
		}
	}
}
}
