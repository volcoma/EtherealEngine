#pragma once
#include <string>
#include <array>

struct extensions 
{
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

    static const std::array<std::string, 6> texture;
    static const std::array<std::string, 5> mesh;
	static std::string shader;
	static std::string material;
	static std::string prefab;
	static std::string scene;
	static std::string compiled;
};
