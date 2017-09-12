#pragma once
#include <array>
#include <string>

#include "core/graphics/graphics.h"
struct shader;
struct scene;
struct prefab;
class material;

namespace extensions
{
const std::array<std::string, 6> texture = {{".png", ".jpg", ".tga", ".dds", ".ktx", ".pvr"}};
const std::array<std::string, 5> mesh = {{".obj", ".fbx", ".dae", ".blend", ".3ds"}};
const std::string shader = ".sc";
const std::string material = ".mat";
const std::string animation = ".anim";
const std::string prefab = ".pfb";
const std::string scene = ".sgr";
const std::string compiled = ".asset";

inline bool is_compiled_format(const std::string& extension)
{
	const bool is_compiled = (extension == extensions::compiled || extension == extensions::material ||
							  extension == extensions::prefab || extension == extensions::scene);
	return is_compiled;
}

template <typename T>
inline std::string get_compiled_format()
{
	return extensions::compiled;
}

template <>
inline std::string get_compiled_format<::shader>()
{
	const auto& renderer_extension = gfx::get_renderer_filename_extension();
	return renderer_extension + extensions::compiled;
}

template <>
inline std::string get_compiled_format<::scene>()
{
	return "";
}

template <>
inline std::string get_compiled_format<::prefab>()
{
	return "";
}

template <>
inline std::string get_compiled_format<::material>()
{
	return "";
}
};
