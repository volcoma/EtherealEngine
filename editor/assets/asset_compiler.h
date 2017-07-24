#pragma once
#include "core/filesystem/filesystem.h"

namespace asset_compiler
{
template <typename T>
extern void compile(const fs::path& absolute_key);
};
