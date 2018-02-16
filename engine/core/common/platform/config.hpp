#pragma once
#include <cstdint>
#include <limits>

#if !defined(DEBUG) && !defined(_DEBUG)
#ifndef NDEBUG
#define NDEBUG
#endif
#ifndef _NDEBUG
#define _NDEBUG
#endif
#endif

// OS utils. Here is where the fun starts... good luck

#define ETH_QUOTE(...) #__VA_ARGS__
#define ETH_COMMENT(...) ETH_NO
#define ETH_UNCOMMENT(...) ETH_YES

#define ETH_YES(...) __VA_ARGS__
#define ETH_NO(...)

#define ETH_ON(v) (0 v(+1)) // usage: #if ETH_ON(ETH_COMPILER_MSVC)
#define ETH_IS ETH_ON		// usage: #if ETH_ON(ETH_DEBUG)
#define ETH_HAS(...)                                                                                         \
	ETH_COMPILER_CLANG(__has_feature(__VA_ARGS__))                                                           \
	ETH_COMPILER_CELSE(__VA_ARGS__) // usage: #if ETH_HAS(cxx_exceptions)

#if defined(_WIN32)
#define ETH_PLATFORM_WINDOWS ETH_YES
#define ETH_PLATFOMR_WELSE ETH_NO
#else
#define ETH_PLATFORM_WINDOWS ETH_NO
#define ETH_PLATFOMR_WELSE ETH_YES
#endif

#ifdef __APPLE__
#define ETH_PLATFORM_APPLE ETH_YES
#define ETH_PLATFORM_AELSE ETH_NO
#else
#define ETH_PLATFORM_APPLE ETH_NO
#define ETH_PLATFORM_AELSE ETH_YES
#endif

#ifdef __linux__
#define ETH_PLATFORM_LINUX ETH_YES
#define ETH_PLATFORM_LELSE ETH_NO
#else
#define ETH_PLATFORM_LINUX ETH_NO
#define ETH_PLATFORM_LELSE ETH_YES
#endif

#ifdef __ANDROID__
#define ETH_PLATFORM_ANDROID ETH_YES
#define ETH_PLATFORM_AELSE ETH_NO
#else
#define ETH_PLATFORM_ANDROID ETH_NO
#define ETH_PLATFORM_AELSE ETH_YES
#endif

// Compiler utils
#if INTPTR_MAX == INT64_MAX
#define ETH_ARCH_64 ETH_YES
#define ETH_ARCH_32 ETH_NO
#else
#define ETH_ARCH_64 ETH_NO
#define ETH_ARCH_32 ETH_YES
#endif

#if defined(NDEBUG) || defined(_NDEBUG) || defined(RELEASE)
#define ETH_DEBUG ETH_YES
#define ETH_RELEASE ETH_NO
#else
#define ETH_DEBUG ETH_NO
#define ETH_RELEASE ETH_YES
#endif

#if defined(NDEVEL) || defined(_NDEVEL) || defined(PUBLIC)
#define ETH_PUBLIC ETH_YES
#define ETH_DEVELOP ETH_NO
#else
#define ETH_PUBLIC ETH_NO
#define ETH_DEVELOP ETH_YES
#endif

#if defined(__GNUC__) || defined(__MINGW32__)
#define ETH_COMPILER_GNUC ETH_YES
#define ETH_COMPILER_GELSE ETH_NO
#else
#define ETH_COMPILER_GNUC ETH_NO
#define ETH_COMPILER_GELSE ETH_YES
#endif

#if defined(__MINGW32__)
#define ETH_COMPILER_MINGW ETH_YES
#define ETH_COMPILER_MIELSE ETH_NO
#else
#define ETH_COMPILER_MINGW ETH_NO
#define ETH_COMPILER_MIELSE ETH_YES
#endif

#ifdef _MSC_VER
#define ETH_COMPILER_MSVC ETH_YES
#define ETH_COMPILER_MELSE ETH_NO
#else
#define ETH_COMPILER_MSVC ETH_NO
#define ETH_COMPILER_MELSE ETH_YES
#endif

#ifdef __clang__
#define ETH_COMPILER_CLANG ETH_YES
#define ETH_COMPILER_CELSE ETH_NO
#else
#define ETH_COMPILER_CLANG ETH_NO
#define ETH_COMPILER_CELSE ETH_YES
#endif

#if ETH_ON(ETH_COMPILER_MSVC) || ETH_ON(ETH_COMPILER_GNUC) || ETH_ON(ETH_COMPILER_CLANG) ||                  \
	ETH_ON(ETH_COMPILER_MINGW)
#define ETH_UNDEFINED_COMPILER ETH_NO
#else
#define ETH_UNDEFINED_COMPILER ETH_YES
#endif

#if ETH_ON(ETH_PLATFORM_WINDOWS) || ETH_ON(ETH_PLATFORM_LINUX) || ETH_ON(ETH_PLATFORM_APPLE) ||              \
	ETH_ON(ETH_PLATFORM_ANDROID)
#define ETH_UNDEFINED_OS ETH_NO
#else
#define ETH_UNDEFINED_OS ETH_YES
#endif

template <bool>
inline bool eval()
{
	return true;
}

template <>
inline bool eval<false>()
{
	return false;
}
#define runtime_eval(_x) eval<!!(_x)>()
