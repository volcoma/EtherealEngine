#pragma once
#include "core/filesystem/filesystem.h"

struct asset_compiler
{
	template<typename T>
	static void compile(const fs::path& absolute_key);
};
