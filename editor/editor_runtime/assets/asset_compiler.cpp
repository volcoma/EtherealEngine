#include "asset_compiler.h"
#include "asset_extensions.h"
#include "mesh_importer.h"

#include <bx/error.h>
#include <bx/process.h>
#include <bx/string.h>

#include <core/audio/loaders/loader.h>
#include <core/audio/sound.h>
#include <core/filesystem/filesystem.h>
#include <core/graphics/graphics.h>
#include <core/graphics/shader.h>
#include <core/graphics/texture.h>
#include <core/logging/logging.h>
#include <core/serialization/associative_archive.h>
#include <core/serialization/binary_archive.h>
#include <core/serialization/serialization.h>
#include <core/serialization/types/map.hpp>
#include <core/serialization/types/unordered_map.hpp>
#include <core/serialization/types/vector.hpp>
#include <core/string_utils/string_utils.h>
#include <core/uuid/uuid.hpp>

#include <runtime/ecs/constructs/prefab.h>
#include <runtime/ecs/constructs/scene.h>
#include <runtime/meta/animation/animation.hpp>
#include <runtime/meta/audio/sound.hpp>
#include <runtime/meta/rendering/material.hpp>
#include <runtime/meta/rendering/mesh.hpp>

#include <array>
#include <fstream>

namespace asset_compiler
{
static std::string escape_str(const std::string& str)
{
	return "\"" + str + "\"";
}

static bool run_compile_process(const std::string& process, const std::vector<std::string>& args_array,
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
#if ETH_ON(ETH_PLATFORM_WINDOWS)
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
		std::array<char, 2048> buffer;
		buffer.fill(0);
		int32_t sz = process_reader.read(buffer.data(), static_cast<std::int32_t>(buffer.size()), &error);

		process_reader.close();
		int32_t result = process_reader.getExitCode();

		if(0 != result)
		{
			err = std::string(error.getMessage().getPtr());
			if(sz > 0)
			{
				err += " " + std::string(buffer.data());
			}
			return false;
		}

		return true;
	}
}

template <>
void compile<gfx::shader>(const fs::path& absolute_meta_key, const fs::path& output)
{
	fs::error_code err;
	fs::path absolute_key = fs::convert_to_protocol(absolute_meta_key);
	absolute_key = fs::resolve_protocol(fs::replace(absolute_key, ":/meta", ":/data"));
	absolute_key.replace_extension();
	std::string str_input = absolute_key.string();
	std::string file = absolute_key.stem().string();
	fs::path dir = absolute_key.parent_path();

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

	auto renderer = gfx::get_renderer_type();
	if(renderer == gfx::renderer_type::Direct3D11 || renderer == gfx::renderer_type::Direct3D12)
	{
		str_platform = "windows";

		if(vs)
			str_profile = "vs_4_0";
		else if(fs)
			str_profile = "ps_4_0";
		else if(cs)
			str_profile = "cs_5_0";
	}
	else if(renderer == gfx::renderer_type::OpenGL)
	{
		str_platform = "linux";

		if(vs || fs)
			str_profile = "120";
		else if(cs)
			str_profile = "430";
	}
	else if(renderer == gfx::renderer_type::Metal)
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
		"-f",		  str_input,	"-o", str_output,  "-i",	 str_include, "--varyingdef", str_varying,
		"--platform", str_platform, "-p", str_profile, "--type", str_type,	"-O",			  "3",
	};

	std::string error;

	{
		std::ofstream output_file(str_output);
		(void)output_file;
	}

	if(!run_compile_process("shaderc", args_array, error))
	{
		APPLOG_ERROR("Failed compilation of {0} with error: {1}", str_input, error);
	}
	else
	{
		APPLOG_INFO("Successful compilation of {0}", str_input);
		fs::copy_file(temp, output, fs::copy_options::overwrite_existing, err);
	}
	fs::remove(temp, err);
}

template <>
void compile<gfx::texture>(const fs::path& absolute_meta_key, const fs::path& output)
{
	fs::error_code err;
	fs::path absolute_key = fs::convert_to_protocol(absolute_meta_key);
	absolute_key = fs::resolve_protocol(fs::replace(absolute_key, ":/meta", ":/data"));
	absolute_key.replace_extension();
	std::string str_input = absolute_key.string();

	fs::path temp = fs::temp_directory_path(err);
	temp /= uuids::random_uuid(str_input).to_string() + ".buildtemp";

	std::string str_output = temp.string();

	const std::vector<std::string> args_array = {
		"-f", str_input, "-o", str_output, "--as", "ktx", "-m", "-t", "BGRA8",
	};

	std::string error;

	{
		std::ofstream output_file(str_output);
		(void)output_file;
	}

	if(!run_compile_process("texturec", args_array, error))
	{
		APPLOG_ERROR("Failed compilation of {0} with error: {1}", str_input, error);
	}
	else
	{
		APPLOG_INFO("Successful compilation of {0}", str_input);
		fs::copy_file(temp, output, fs::copy_options::overwrite_existing, err);
	}
	fs::remove(temp, err);
}

template <>
void compile<mesh>(const fs::path& absolute_meta_key, const fs::path& output)
{
	fs::error_code err;
	fs::path absolute_key = fs::convert_to_protocol(absolute_meta_key);
	absolute_key = fs::resolve_protocol(fs::replace(absolute_key, ":/meta", ":/data"));
	absolute_key.replace_extension();
	std::string str_input = absolute_key.string();

	fs::path temp = fs::temp_directory_path(err);
	temp /= uuids::random_uuid(str_input).to_string() + ".buildtemp";

	mesh::load_data data;
	std::vector<runtime::animation> animations;
	if(!importer::load_mesh_data_from_file(str_input, data, animations))
	{
		APPLOG_ERROR("Failed compilation of {0}", str_input);
		return;
	}

	if(!data.vertex_data.empty())
	{
		{
			std::ofstream soutput(temp.string(), std::ios::out | std::ios::binary);
			cereal::oarchive_binary_t ar(soutput);
			try_save(ar, cereal::make_nvp("mesh", data));
		}
		fs::copy_file(temp, output, fs::copy_options::overwrite_existing, err);
		fs::remove(temp, err);

		APPLOG_INFO("Successful compilation of {0}", str_input);
	}
	{
		fs::path file = absolute_key.stem();
		fs::path dir = absolute_key.parent_path();

		for(const auto& animation : animations)
		{
			temp = fs::temp_directory_path(err);
			temp.append(uuids::random_uuid(str_input).to_string() + ".buildtemp");
			{
				std::ofstream soutput(temp.string(), std::ios::out | std::ios::binary);
				cereal::oarchive_associative_t ar(soutput);
				try_save(ar, cereal::make_nvp("animation", animation));
			}
			fs::path anim_output = (dir / file).string() + "_" + animation.name + ".anim";

			fs::copy_file(temp, anim_output, fs::copy_options::overwrite_existing, err);
			fs::remove(temp, err);

			APPLOG_INFO("Successful compilation of animation {0}", animation.name);
		}
	}
}

template <>
void compile<runtime::animation>(const fs::path& absolute_meta_key, const fs::path& output)
{
	fs::error_code err;
	fs::path absolute_key = fs::convert_to_protocol(absolute_meta_key);
	absolute_key = fs::resolve_protocol(fs::replace(absolute_key, ":/meta", ":/data"));
	absolute_key.replace_extension();
	std::string str_input = absolute_key.string();

	bool has_loaded = false;
	runtime::animation anim;
	{
		std::ifstream stream(absolute_key.string());
		if(stream.good())
		{
			cereal::iarchive_associative_t ar(stream);

			try_load(ar, cereal::make_nvp("animation", anim));

			has_loaded = true;
		}
	}

	if(has_loaded)
	{
		std::ofstream stream(output.string(), std::ios::binary);
		if(stream.good())
		{
			cereal::oarchive_binary_t ar(stream);

			try_save(ar, cereal::make_nvp("animation", anim));

			APPLOG_INFO("Successful compilation of {0}", str_input);
		}
	}
}

template <>
void compile<audio::sound>(const fs::path& absolute_meta_key, const fs::path& output)
{
	fs::error_code err;
	fs::path absolute_key = fs::convert_to_protocol(absolute_meta_key);
	absolute_key = fs::resolve_protocol(fs::replace(absolute_key, ":/meta", ":/data"));
	absolute_key.replace_extension();

	std::string str_input = absolute_key.string();

	fs::path temp = fs::temp_directory_path(err);
	temp /= uuids::random_uuid(str_input).to_string() + ".buildtemp";

	std::ifstream f(str_input, std::ios::in | std::ios::binary);

	if(!f.is_open())
	{
		APPLOG_ERROR("Cant open file {0}", str_input);
		return;
	}

	auto file_data = fs::read_stream(f);

	auto ext = absolute_key.extension();
	audio::sound_data data;

	if(ext == ".ogg")
	{
		std::string load_err;
		if(!audio::load_ogg_from_memory(file_data.data(), file_data.size(), data, load_err))
		{
			APPLOG_ERROR("Failed compilation of {0} with error : {1}", str_input, load_err);
			return;
		}
	}
	else if(ext == ".wav")
	{
		std::string load_err;
		if(!audio::load_wav_from_memory(file_data.data(), file_data.size(), data, load_err))
		{
			APPLOG_ERROR("Failed compilation of {0} with error : {1}", str_input, load_err);
			return;
		}
	}
	else
	{
		APPLOG_ERROR("Failed compilation of {0} with error : Unsupported", str_input);
		return;
	}

	{
		std::ofstream soutput(temp.string(), std::ios::out | std::ios::binary);
		cereal::oarchive_binary_t ar(soutput);
		try_save(ar, cereal::make_nvp("sound", data));
	}
	fs::copy_file(temp, output, fs::copy_options::overwrite_existing, err);
	fs::remove(temp, err);

	APPLOG_INFO("Successful compilation of {0}", str_input);
}

template <>
void compile<material>(const fs::path& absolute_meta_key, const fs::path& output)
{
	fs::error_code err;
	fs::path absolute_key = fs::convert_to_protocol(absolute_meta_key);
	absolute_key = fs::resolve_protocol(fs::replace(absolute_key, ":/meta", ":/data"));
	absolute_key.replace_extension();
	std::string str_input = absolute_key.string();

	std::shared_ptr<::material> material;
	{
		std::ifstream stream(absolute_key.string());
		if(stream.good())
		{
			cereal::iarchive_associative_t ar(stream);

			try_load(ar, cereal::make_nvp("material", material));
		}
	}

	if(material)
	{
		std::ofstream stream(output.string(), std::ios::binary);
		if(stream.good())
		{
			cereal::oarchive_binary_t ar(stream);

			try_save(ar, cereal::make_nvp("material", material));

			APPLOG_INFO("Successful compilation of {0}", str_input);
		}
	}
}

template <>
void compile<prefab>(const fs::path& absolute_meta_key, const fs::path& output)
{
	fs::error_code err;
	fs::path absolute_key = fs::convert_to_protocol(absolute_meta_key);
	absolute_key = fs::resolve_protocol(fs::replace(absolute_key, ":/meta", ":/data"));
	absolute_key.replace_extension();
	fs::copy_file(absolute_key, output, fs::copy_options::overwrite_existing, err);
	APPLOG_INFO("Successful compilation of {0}", absolute_key.string());
}

template <>
void compile<scene>(const fs::path& absolute_meta_key, const fs::path& output)
{
	fs::error_code err;
	fs::path absolute_key = fs::convert_to_protocol(absolute_meta_key);
	absolute_key = fs::resolve_protocol(fs::replace(absolute_key, ":/meta", ":/data"));
	absolute_key.replace_extension();
	fs::copy_file(absolute_key, output, fs::copy_options::overwrite_existing, err);
	APPLOG_INFO("Successful compilation of {0}", absolute_key.string());
}
}
