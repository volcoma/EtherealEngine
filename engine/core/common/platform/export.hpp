#pragma once

#include "config.hpp"

////////////////////////////////////////////////////////////
// Define helpers to create portable import / export macros for each module
////////////////////////////////////////////////////////////
#if !defined(ETH_STATIC)

#if ETH_ON(ETH_PLATFORM_WINDOWS)
// Windows compilers need specific (and different) keywords for export and import
#define ETH_API_EXPORT __declspec(dllexport)
#define ETH_API_IMPORT __declspec(dllimport)

// For Visual C++ compilers, we also need to turn off this annoying C4251 warning
#if ETH_ON(ETH_COMPILER_MSVC)
#pragma warning(disable : 4251)
#endif

#else // Linux, FreeBSD, Mac OS X

// GCC 4+ has special keywords for showing/hidding symbols,
// the same keyword is used for both importing and exporting
#define ETH_API_EXPORT __attribute__((__visibility__("default")))
#define ETH_API_IMPORT __attribute__((__visibility__("default")))

#endif

#else

// Static build doesn't need import/export macros
#define ETH_API_EXPORT
#define ETH_API_IMPORT

#endif

////////////////////////////////////////////////////////////
// Define portable import / export macros
////////////////////////////////////////////////////////////
#if defined(ETH_API_EXPORTS)
#define ETH_API ETH_API_EXPORT
#else
#define ETH_API ETH_API_IMPORT
#endif
