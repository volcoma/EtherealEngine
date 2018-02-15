#pragma once
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
#define $quote(...) #__VA_ARGS__
#define $comment(...) $no
#define $uncomment(...) $yes

#define ETH_QUOTE(...) #__VA_ARGS__
#define ETH_COMMENT(...) ETH_NO
#define ETH_UNCOMMENT(...) ETH_YES

#define $yes(...) __VA_ARGS__
#define $no(...)

#define ETH_YES(...) __VA_ARGS__
#define ETH_NO(...)

#define $on(v) (0 v(+1))												 // usage: #if $on($msvc)
#define $is $on															 // usage: #if $is($debug)
#define $has(...) $clang(__has_feature(__VA_ARGS__)) $celse(__VA_ARGS__) // usage: #if $has(cxx_exceptions)

#define ETH_ON(v) (0 v(+1))												 // usage: #if $on($msvc)
#define ETH_IS ETH_ON															 // usage: #if $is($debug)
#define ETH_HAS(...) ETH_COMPILER_CLANG(__has_feature(__VA_ARGS__)) ETH_COMPILER_CELSE(__VA_ARGS__) // usage: #if $has(cxx_exceptions)


#if defined(_WIN32)
#define $windows $yes
#define $welse $no
#define ETH_PLATFORM_WINDOWS ETH_YES
#define ETH_PLATFOMR_WELSE ETH_NO
#else
#define $windows $no
#define $welse $yes
#define ETH_PLATFORM_WINDOWS ETH_NO
#define ETH_PLATFOMR_WELSE ETH_YES
#endif

#ifdef __APPLE__
#define $apple $yes
#define $aelse $no
#define ETH_APPLE ETH_YES
#define ETH_AELSE ETH_NO
#else
#define $apple $no
#define $aelse $yes
#define ETH_PLATFORM_APPLE ETH_NO
#define ETH_PLATFORM_AELSE ETH_YES
#endif

#ifdef __linux__
#define $linux $yes
#define $lelse $no
#else
#define $linux $no
#define $lelse $yes
#endif

#ifdef __ANDROID__
#define $android $yes
#define $aelse $no
#else
#define $android $no
#define $aelse $yes
#endif

// Compiler utils

#if ULONG_MAX == 4294967295
#define $bits64 $yes
#define $bits32 $no
#else
#define $bits64 $no
#define $bits32 $yes
#endif

#if defined(NDEBUG) || defined(_NDEBUG) || defined(RELEASE)
#define $release $yes
#define $debug $no
#else
#define $release $no
#define $debug $yes
#endif

#if defined(NDEVEL) || defined(_NDEVEL) || defined(PUBLIC)
#define $public $yes
#define $devel $no
#else
#define $public $no
#define $devel $yes
#endif

#if defined(__GNUC__) || defined(__MINGW32__)
#define $gnuc $yes
#define $gelse $no
#else
#define $gnuc $no
#define $gelse $yes
#endif

#if defined(__MINGW32__)
#define $mingw $yes
#else
#define $mingw $no
#endif

#ifdef _MSC_VER
#define $msvc $yes
#define $melse $no
#else
#define $msvc $no
#define $melse $yes
#endif

#ifdef __clang__
#define $clang $yes
#define $celse $no
#else
#define $clang $no
#define $celse $yes
#endif

#if $on($msvc) || $on($gnuc) || $on($clang)
#define $undefined_compiler $no
#else
#define $undefined_compiler $yes
#endif

#if $on($windows) || $on($linux) || $on($apple) || $on($android)
#define $undefined_os $no
#else
#define $undefined_os $yes
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
