#pragma once
#include "runtime/system/filesystem.h"

struct Shader;
struct Texture;
struct Mesh;

struct ShaderCompiler
{
	static void compile(const fs::path& absoluteKey);
};

struct TextureCompiler
{
	static void compile(const fs::path& absoluteKey);
};

struct MeshCompiler
{
	static void compile(const fs::path& absoluteKey);
};

template<typename T>
struct AssetCompiler
{
	static void compile(const fs::path& absoluteKey);
};

template<>
struct AssetCompiler<Shader>
{
	static void compile(const fs::path& absoluteKey)
	{
		ShaderCompiler::compile(absoluteKey);
	}
};

template<>
struct AssetCompiler<Texture>
{
	static void compile(const fs::path& absoluteKey)
	{
		TextureCompiler::compile(absoluteKey);
	}
};

template<>
struct AssetCompiler<Mesh>
{
	static void compile(const fs::path& absoluteKey)
	{
		MeshCompiler::compile(absoluteKey);
	}
};