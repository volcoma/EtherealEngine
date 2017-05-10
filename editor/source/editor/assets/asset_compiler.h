#pragma once
#include "runtime/system/filesystem.h"

struct shader;
struct texture;
class mesh;

struct shader_compiler
{
	static void compile(const fs::path& absoluteKey);
};

struct texture_compiler
{
	static void compile(const fs::path& absoluteKey);
};

struct mesh_compiler
{
	static void compile(const fs::path& absoluteKey);
};

template<typename T>
struct asset_compiler
{
	static void compile(const fs::path& absoluteKey);
};

template<>
struct asset_compiler<shader>
{
	static void compile(const fs::path& absoluteKey)
	{
		shader_compiler::compile(absoluteKey);
	}
};

template<>
struct asset_compiler<texture>
{
	static void compile(const fs::path& absoluteKey)
	{
		texture_compiler::compile(absoluteKey);
	}
};

template<>
struct asset_compiler<mesh>
{
	static void compile(const fs::path& absoluteKey)
	{
		mesh_compiler::compile(absoluteKey);
	}
};