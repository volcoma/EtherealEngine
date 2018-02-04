#pragma once
#include <string>
#include <vector>

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
