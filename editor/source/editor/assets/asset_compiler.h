#pragma once
#include "runtime/system/filesystem.h"

struct ShaderCompiler
{
	static void compile(const fs::path& absoluteKey);
};
