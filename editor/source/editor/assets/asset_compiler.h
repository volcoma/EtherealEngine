#pragma once
#include "runtime/system/FileSystem.h"

struct ShaderCompiler
{
	void compile(const fs::path& absoluteKey);
};