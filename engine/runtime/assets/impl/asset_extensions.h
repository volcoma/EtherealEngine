#pragma once
#include <array>
#include <string>
#include <vector>

#include "core/graphics/graphics.h"

namespace gfx
{
struct shader;
}
namespace audio
{
class sound;
}
struct scene;
struct prefab;
class material;
namespace runtime
{
struct animation;
}
namespace extensions
{
const std::array<std::string, 7> texture = {{".png", ".jpg", ".jpeg", ".tga", ".dds", ".ktx", ".pvr"}};
const std::array<std::string, 5> mesh = {{".obj", ".fbx", ".dae", ".blend", ".3ds"}};
const std::array<std::string, 2> sound = {{".ogg", ".wav"}};
const std::string shader = ".sc";
const std::string material = ".mat";
const std::string animation = ".anim";
const std::string prefab = ".pfb";
const std::string scene = ".sgr";
const std::string compiled = ".asset";

inline bool is_compiled_format(const std::string& extension)
{
	const bool is_compiled = (extension == extensions::compiled || extension == extensions::material ||
							  extension == extensions::animation || extension == extensions::prefab ||
							  extension == extensions::scene);
	return is_compiled;
}

template <typename T>
inline std::string get_compiled_format()
{
	return extensions::compiled;
}

template <>
inline std::string get_compiled_format<gfx::shader>()
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
template <>
inline std::string get_compiled_format<runtime::animation>()
{
	return "";
}
}

namespace ex
{
inline const std::vector<std::string>& get_suported_texture_formats()
{
	static std::vector<std::string> formats = {".png", ".jpg", ".jpeg", ".tga", ".dds", ".ktx", ".pvr"};
	return formats;
}

inline const std::vector<std::string>& get_suported_mesh_formats()
{
	static std::vector<std::string> formats = {".obj", ".fbx", ".dae", ".blend", ".3ds"};
	return formats;
}

inline const std::vector<std::string>& get_suported_sound_formats()
{
	static std::vector<std::string> formats = {".ogg", ".wav"};
	return formats;
}

inline const std::vector<std::string>& get_suported_shader_formats()
{
	static std::vector<std::string> formats = {".sc"};
	return formats;
}

inline const std::vector<std::string>& get_suported_material_formats()
{
	static std::vector<std::string> formats = {".mat"};
	return formats;
}

inline const std::vector<std::string>& get_suported_animation_formats()
{
	static std::vector<std::string> formats = {".anim"};
	return formats;
}

inline const std::vector<std::string>& get_suported_prefab_formats()
{
	static std::vector<std::string> formats = {".pfb"};
	return formats;
}

inline const std::vector<std::string>& get_suported_scene_formats()
{
	static std::vector<std::string> formats = {".sgr"};
	return formats;
}
}
