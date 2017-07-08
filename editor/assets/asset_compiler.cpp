#include "asset_compiler.h"
#include "core/common/string.h"
#include "core/logging/logging.h"
#include "core/filesystem/filesystem.h"
#include "core/serialization/serialization.h"
#include "core/serialization/binary_archive.h"
#include "core/serialization/types/unordered_map.hpp"
#include "core/serialization/types/vector.hpp"
#include "runtime/assets/asset_extensions.h"
#include "runtime/meta/rendering/mesh.hpp"
#include "runtime/rendering/shader.h"
#include "runtime/rendering/texture.h"
#include "mesh_importer.h"
#include "shaderc/shaderc.h"
#include "texturec/texturec.h"
#include "core/graphics/graphics.h"
#include <fstream>
#include <array>


class material;
struct prefab;
struct scene;

template<>
void asset_compiler::compile<prefab>(const fs::path& absolute_key)
{
}

template<>
void asset_compiler::compile<scene>(const fs::path& absolute_key)
{
}

template<>
void asset_compiler::compile<material>(const fs::path& absolute_key)
{
}

template<>
void asset_compiler::compile<shader>(const fs::path& absolute_key)
{
	const auto& renderer_extension = gfx::get_renderer_filename_extension();
	fs::path output = absolute_key;
	output.concat(renderer_extension);
	output.concat(extensions::compiled);
	std::string str_input = absolute_key.string();
	std::string file = absolute_key.stem().string();
	fs::path dir = absolute_key.parent_path();
	fs::path temp = dir / fs::path(file + ".buildtemp");

	bool vs = string_utils::begins_with(file, "vs_");
	bool fs = string_utils::begins_with(file, "fs_");
	bool cs = string_utils::begins_with(file, "cs_");
	
	std::string str_output = temp.string();
	static const int arg_count = 16;
	const char* args_array[arg_count];
	args_array[0] = "-f";
	args_array[1] = str_input.c_str();
	args_array[2] = "-o";
	args_array[3] = str_output.c_str();
	args_array[4] = "-i";
	fs::path include = fs::resolve_protocol("engine_data:/shaders");
	std::string str_include = include.string();
	args_array[5] = str_include.c_str();
	args_array[6] = "--varyingdef";
	fs::path varying = dir / (file + ".io");
	std::string str_varying = varying.string();
	args_array[7] = str_varying.c_str();
	args_array[8] = "--platform";

	auto renderer = gfx::getRendererType();
	if (renderer == gfx::RendererType::Direct3D11 ||
		renderer == gfx::RendererType::Direct3D12)
	{
		args_array[9] = "windows";
		args_array[10] = "-p";

		if (vs)
			args_array[11] = "vs_4_0";
		else if (fs)
			args_array[11] = "ps_4_0";
		else if (cs)
			args_array[11] = "cs_5_0";
	}
	else if (renderer == gfx::RendererType::OpenGL)
	{
		args_array[9] = "linux";
		args_array[10] = "-p";

		if (vs || fs)
			args_array[11] = "120";
		else if (cs)
			args_array[11] = "430";
	}
	else if (renderer == gfx::RendererType::Metal)
	{
		args_array[9] = "osx";
		args_array[10] = "-p";
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
	

	fs::error_code err;
	int result = -1;
	if (renderer == gfx::RendererType::OpenGL)
	{
		static std::mutex mtx;
		std::lock_guard<std::mutex> lock(mtx);
        result = compile_shader(arg_count, args_array);
	}
	else
	{
		result = compile_shader(arg_count, args_array);
	}
	
	if (result != 0)
	{
		APPLOG_ERROR("Failed compilation of {0}", str_input);
	}
	else
	{
		APPLOG_INFO("Successful compilation of {0}", str_input);
		fs::copy_file(temp, output, fs::copy_option::overwrite_if_exists, err);
		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		fs::last_write_time(output, now, err);
	}
	fs::remove(temp, err);
}

template<>
void asset_compiler::compile<texture>(const fs::path& absolute_key)
{
	fs::path output = absolute_key;
	output.concat(extensions::compiled);
	std::string str_input = absolute_key.string();
	std::string raw_ext = absolute_key.filename().extension().string();
	std::string file = absolute_key.stem().string();

	std::string str_output = output.string();
    fs::error_code err;
	if (raw_ext == ".dds" || raw_ext == ".pvr" || raw_ext == ".ktx")
	{
		fs::copy_file(absolute_key, output, fs::copy_option::overwrite_if_exists, err);
		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		fs::last_write_time(output, now, err);
		APPLOG_INFO("Successful compilation of {0}", str_input);
		return;
	}


	static const int arg_count = 7;
	const char* args_array[arg_count];
	args_array[0] = "-f";
	args_array[1] = str_input.c_str();
	args_array[2] = "-o";
	args_array[3] = str_output.c_str();
	args_array[4] = "--as";
	args_array[5] = "ktx";
	args_array[6] = "-m";
	

	if (compile_texture(arg_count, args_array) != 0)
	{
		APPLOG_ERROR("Failed compilation of {0}", str_input);
	}
	else
	{
		APPLOG_INFO("Successful compilation of {0}", str_input);
		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		fs::last_write_time(output, now, err);
	}
}

template<>
void asset_compiler::compile<mesh>(const fs::path& absolute_key)
{
	fs::path output = absolute_key;
	output.concat(extensions::compiled);
	std::string str_input = absolute_key.string();
	std::string file = absolute_key.stem().string();
	fs::path dir = absolute_key.parent_path();

	mesh::load_data data;
	if (!importer::load_mesh_data_from_file(str_input, data))
	{
		APPLOG_ERROR("Failed compilation of {0}", str_input);
		return;
	}

	fs::path entry = dir / fs::path(file + ".buildtemp");
	{
        std::ofstream soutput(entry.string(), std::ios::out | std::ios::binary);
		cereal::oarchive_binary_t ar(soutput);
		try_save(ar, cereal::make_nvp("mesh", data));
	}
    fs::error_code err;
    fs::copy_file(entry, output, fs::copy_option::overwrite_if_exists, err);
    fs::remove(entry, err);

	APPLOG_INFO("Successful compilation of {0}", str_input);

}
