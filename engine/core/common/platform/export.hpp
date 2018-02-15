#pragma once

#include "config.hpp"

////////////////////////////////////////////////////////////
// Define helpers to create portable import / export macros for each module
////////////////////////////////////////////////////////////
#if !defined(ETH_STATIC)

#if $on($windows)
// Windows compilers need specific (and different) keywords for export and import
#define ETH_API_EXPORT __declspec(dllexport)
#define ETH_API_IMPORT __declspec(dllimport)

// For Visual C++ compilers, we also need to turn off this annoying C4251 warning
#if $on($msvc)
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
// Cross-platform warning for deprecated functions and classes
//
// Usage:
// class ETH_DEPRECATED MyClass
// {
//     ETH_DEPRECATED void memberFunc();
// };
//
// ETH_DEPRECATED void globalFunc();
////////////////////////////////////////////////////////////
#if defined(ETH_NO_DEPRECATED_WARNINGS)

// User explicitly requests to disable deprecation warnings
#define ETH_DEPRECATED

#elif $on($msvc)

// Microsoft C++ compiler
// Note: On newer MSVC versions, using deprecated functions causes a compiler error. In order to
// trigger a warning instead of an error, the compiler flag /sdl- (instead of /sdl) must be specified.
#define ETH_DEPRECATED __declspec(deprecated)

#elif $on($gnuc) || $on($clang)

// g++ and clang
#define ETH_DEPRECATED __attribute__((deprecated))

#else

// Other compilers are not supported, leave class or function as-is.
// With a bit of luck, the #pragma directive works, otherwise users get a warning (no error!) for unrecognized
// #pragma.
#pragma message("ETH_DEPRECATED is not supported for your compiler.")
#define ETH_DEPRECATED

#endif


////////////////////////////////////////////////////////////
// Define portable import / export macros
////////////////////////////////////////////////////////////
#if defined(ETH_API_EXPORTS)
    #define ETH_API ETH_API_EXPORT
#else
    #define ETH_API ETH_API_IMPORT
#endif
