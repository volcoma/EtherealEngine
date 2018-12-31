#pragma once

#if defined(__cplusplus) && __cplusplus >= 201703L
#if defined(__has_include) && __has_include(<filesystem>)
#include <filesystem>
namespace fs
{
using namespace std::filesystem;
using error_code = std::error_code;
inline file_time_type now()
{
	return std::chrono::system_clock::now();
}
} // namespace fs
#endif
#else
#include "filesystem_impl.hpp"

namespace fs
{
using namespace ghc::filesystem;
using error_code = std::error_code;
inline file_time_type now()
{
	return std::chrono::system_clock::now();
}
} // namespace fs
#endif
