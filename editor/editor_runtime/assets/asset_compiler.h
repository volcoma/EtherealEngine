#pragma once
#include "core/filesystem/filesystem.h"

namespace project_compiler
{
void compile_texture(const fs::path& absolute_meta_key, const fs::path& output);
void compile_mesh(const fs::path& absolute_meta_key, const fs::path& output);
void compile_animation(const fs::path& absolute_meta_key, const fs::path& output);
void compile_sound(const fs::path& absolute_meta_key, const fs::path& output);
void compile_shader(const fs::path& absolute_meta_key, const fs::path& output);
void compile_material(const fs::path& absolute_meta_key, const fs::path& output);
void compile_scene(const fs::path& absolute_meta_key, const fs::path& output);
void compile_prefab(const fs::path& absolute_meta_key, const fs::path& output);
};
