#include "asset_compiler.h"
#include "core/common/string.h"
#include "core/logging/logging.h"
#include "runtime/system/filesystem.h"
#include "shaderc/shaderc.h"
#include "texturec/texturec.h"
#include "geometryc/geometryc.h"
#include <fstream>
#include <array>
#include "graphics/graphics.h"

#include "runtime/assets/asset_extensions.h"
#include "core/serialization/serialization.h"
#include "core/serialization/archives.h"
#include "core/serialization/cereal/types/unordered_map.hpp"
#include "core/serialization/cereal/types/vector.hpp"

void ShaderCompiler::compile(const fs::path& absoluteKey)
{
	std::string strInput = absoluteKey.string();
	std::string file = absoluteKey.stem().string();
	fs::path dir = absoluteKey.parent_path();
	fs::path output = dir / fs::path(file + extensions::shader);

	std::array<gfx::RendererType::Enum, 4> supported =
	{
		gfx::RendererType::Direct3D9,
		gfx::RendererType::Direct3D11,
		gfx::RendererType::OpenGL,
		gfx::RendererType::Metal
	};

	bool vs = string_utils::begins_with(file, "vs_");
	bool fs = string_utils::begins_with(file, "fs_");
	bool cs = string_utils::begins_with(file, "cs_");

	std::unordered_map<gfx::RendererType::Enum, fs::byte_array_t> binaries;
	binaries.reserve(4);
	std::string strOutput = output.string();
	for (auto& platform : supported)
	{
		static const int arg_count = 16;
		const char* args_array[arg_count];
		args_array[0] = "-f";
		args_array[1] = strInput.c_str();
		args_array[2] = "-o";
		args_array[3] = strOutput.c_str();
		args_array[4] = "-i";
		fs::path include = fs::resolve_protocol("engine:/tools/include");
		std::string strInclude = include.string();
		args_array[5] = strInclude.c_str();
		args_array[6] = "--varyingdef";
		fs::path varying = dir / (file + ".io");
		std::string strVarying = varying.string();
		args_array[7] = strVarying.c_str();
		args_array[8] = "--platform";

		if(platform == gfx::RendererType::Direct3D9 || platform == gfx::RendererType::Direct3D11)
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
		else if (platform == gfx::RendererType::OpenGL)
		{
			args_array[9] = "linux";
			args_array[10] = "-p";

			if (vs || fs)
				args_array[11] = "120";
			else if (cs)
				args_array[11] = "430";
		}
		else if (platform == gfx::RendererType::Metal)
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

		auto logger = logging::get("Log");
		
		bx::CrtAllocator allocator;
		bx::MemoryBlock memBlock(&allocator);
		int64_t sz;
		if (platform != gfx::RendererType::Direct3D9 && platform != gfx::RendererType::Direct3D11)
		{
			//glsl shader compilation is not thread safe-
			static std::mutex mtx;
			std::lock_guard<std::mutex> lock(mtx);
			if (compile_shader(arg_count, args_array, memBlock, sz) != 0)
			{
				logger->error().write("Failed compilation of {0}", strInput);
				return;
			}
		}
		else
		{
			if (compile_shader(arg_count, args_array, memBlock, sz) != 0)
			{
				logger->error().write("Failed compilation of {0}", strInput);
				return;
			}
		}

		if (sz > 0)
		{
			auto buf = (char*)memBlock.more();
			auto length = sz;
			fs::byte_array_t vec;
			std::copy(buf, buf + length, std::back_inserter(vec));
			binaries[platform] = vec;
		}
		
	}

	fs::path entry = dir / fs::path(file + ".buildtemp");
	{		
		std::ofstream soutput(entry);
		cereal::oarchive_json_t ar(soutput, cereal::oarchive_json_t::Options::NoIndent());
		try_save(ar, cereal::make_nvp("shader", binaries));
	}
	fs::copy(entry, output, fs::copy_options::overwrite_existing, std::error_code{});
	fs::remove(entry, std::error_code{});
}


void TextureCompiler::compile(const fs::path& absoluteKey)
{
	auto logger = logging::get("Log");
	std::string strInput = absoluteKey.string();
	std::string raw_ext = absoluteKey.filename().extension().string();
	std::string file = absoluteKey.stem().string();
	fs::path output = absoluteKey.parent_path();
	output /= fs::path(file + extensions::texture);

	std::string strOutput = output.string();

	if (raw_ext == ".dds" || raw_ext == ".pvr" || raw_ext == ".ktx")
	{
		if (!fs::copy_file(strInput, strOutput, fs::copy_options::overwrite_existing, std::error_code{}))
		{
			logger->error().write("Failed compilation of {0}", strInput);
		}
		else
		{
			fs::last_write_time(strOutput, fs::file_time_type::clock::now(), std::error_code{});
		}
		return;
	}


	static const int arg_count = 5;
	const char* args_array[arg_count];
	args_array[0] = "-f";
	args_array[1] = strInput.c_str();
	args_array[2] = "-o";
	args_array[3] = strOutput.c_str();
	args_array[4] = "-m";

	

	if (compile_texture(arg_count, args_array) != 0)
	{
		logger->error().write("Failed compilation of {0}", strInput);
	}
}

void MeshCompiler::compile(const fs::path& absoluteKey)
{
	std::string strInput = absoluteKey.string();
	std::string file = absoluteKey.stem().string();
	fs::path output = absoluteKey.parent_path();
	output /= fs::path(file + extensions::mesh);

	std::string strOutput = output.string();

	static const int arg_count = 10;
	const char* args_array[arg_count];
	args_array[0] = "-f";
	args_array[1] = strInput.c_str();
	args_array[2] = "-o";
	args_array[3] = strOutput.c_str();
	args_array[4] = "--tangent";
	args_array[5] = "--barycentric";
	args_array[6] = "--packnormal";
	args_array[7] = "1";
	args_array[8] = "--packuv";
	args_array[9] = "1";
	
	auto logger = logging::get("Log");
	if (compile_mesh(arg_count, args_array) != 0)
	{
		logger->error().write("Failed compilation of {0}", strInput);
	}
}
