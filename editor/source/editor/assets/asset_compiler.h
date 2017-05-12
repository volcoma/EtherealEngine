#pragma once
#include "runtime/system/filesystem.h"

struct asset_compiler
{
	template<typename T>
	static void compile(const fs::path& absoluteKey);
};
