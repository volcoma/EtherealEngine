#pragma once
#include <string>
#include <array>

#include "core/graphics/graphics.h"
struct shader;
struct scene;
struct prefab;
class material;

struct extensions 
{
	static const std::array<std::string, 6> texture;
	static const std::array<std::string, 5> mesh;
	static const std::string shader;
	static const std::string material;
	static const std::string prefab;
	static const std::string scene;
	static const std::string compiled;

	static bool is_compiled_format(const std::string& extension)
	{
		const bool is_compiled =
		(
			extension == extensions::compiled ||
			extension == extensions::material ||
			extension == extensions::prefab ||
			extension == extensions::scene
		);
		return is_compiled;
	}

	static bool is_has_compiled_format(const std::string& extension)
	{
		const bool is_compiled =
			(
				extension == extensions::compiled ||
				extension == extensions::material ||
				extension == extensions::prefab ||
				extension == extensions::scene
				);
		return is_compiled;
	}

	template<typename T>
	static std::string get_compiled_format()
	{
		return extensions::compiled;
	}

	template<>
	static std::string get_compiled_format<::shader>()
	{
		const auto& renderer_extension = gfx::get_renderer_filename_extension();
		return renderer_extension + extensions::compiled;
	}

	template<>
	static std::string get_compiled_format<::scene>()
	{
		return "";
	}

	template<>
	static std::string get_compiled_format<::prefab>()
	{
		return "";
	}

	template<>
	static std::string get_compiled_format<::material>()
	{
		return "";
	}
};
