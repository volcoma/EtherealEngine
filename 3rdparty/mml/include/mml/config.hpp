#ifndef MML_CONFIG_HPP
#define MML_CONFIG_HPP

////////////////////////////////////////////////////////////
// Identify the operating system
// see http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
////////////////////////////////////////////////////////////
#if defined(_WIN32)

    // Windows
    #define MML_SYSTEM_WINDOWS
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

#elif defined(__APPLE__) && defined(__MACH__)

    // Apple platform, see which one it is
    #include "TargetConditionals.h"

    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR

        // iOS
        #define MML_SYSTEM_IOS

    #elif TARGET_OS_MAC

        // MacOS
        #define MML_SYSTEM_MACOS

    #else

        // Unsupported Apple system
        #error This Apple operating system is not supported by SFML library

    #endif

#elif defined(__unix__)

    // UNIX system, see which one it is
    #if defined(__ANDROID__)

        // Android
        #define MML_SYSTEM_ANDROID

    #elif defined(__linux__)

         // Linux
        #define MML_SYSTEM_LINUX

    #elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)

        // FreeBSD
        #define MML_SYSTEM_FREEBSD

    #else

        // Unsupported UNIX system
        #error This UNIX operating system is not supported by SFML library

    #endif

#else

    // Unsupported system
    #error This operating system is not supported by SFML library

#endif


////////////////////////////////////////////////////////////
// Define a portable debug macro
////////////////////////////////////////////////////////////
#if !defined(NDEBUG)

    #define MML_DEBUG

#endif


////////////////////////////////////////////////////////////
// Define helpers to create portable import / export macros for each module
////////////////////////////////////////////////////////////
#if !defined(MML_STATIC)

    #if defined(MML_SYSTEM_WINDOWS)

        // Windows compilers need specific (and different) keywords for export and import
        #define MML_API_EXPORT __declspec(dllexport)
        #define MML_API_IMPORT __declspec(dllimport)

        // For Visual C++ compilers, we also need to turn off this annoying C4251 warning
        #ifdef _MSC_VER

            #pragma warning(disable: 4251)

        #endif

    #else // Linux, FreeBSD, Mac OS X

        #if __GNUC__ >= 4

            // GCC 4 has special keywords for showing/hidding symbols,
            // the same keyword is used for both importing and exporting
            #define MML_API_EXPORT __attribute__ ((__visibility__ ("default")))
            #define MML_API_IMPORT __attribute__ ((__visibility__ ("default")))

        #else

            // GCC < 4 has no mechanism to explicitely hide symbols, everything's exported
            #define MML_API_EXPORT
            #define MML_API_IMPORT

        #endif

    #endif

#else

    // Static build doesn't need import/export macros
    #define MML_API_EXPORT
    #define MML_API_IMPORT

#endif


////////////////////////////////////////////////////////////
// Cross-platform warning for deprecated functions and classes
//
// Usage:
// class MML_DEPRECATED MyClass
// {
//     MML_DEPRECATED void memberFunc();
// };
//
// MML_DEPRECATED void globalFunc();
////////////////////////////////////////////////////////////
#if defined(MML_NO_DEPRECATED_WARNINGS)

    // User explicitly requests to disable deprecation warnings
    #define MML_DEPRECATED

#elif defined(_MSC_VER)

    // Microsoft C++ compiler
    // Note: On newer MSVC versions, using deprecated functions causes a compiler error. In order to
    // trigger a warning instead of an error, the compiler flag /sdl- (instead of /sdl) must be specified.
    #define MML_DEPRECATED __declspec(deprecated)

#elif defined(__GNUC__)

    // g++ and Clang
    #define MML_DEPRECATED __attribute__ ((deprecated))

#else

    // Other compilers are not supported, leave class or function as-is.
    // With a bit of luck, the #pragma directive works, otherwise users get a warning (no error!) for unrecognized #pragma.
    #pragma message("MML_DEPRECATED is not supported for your compiler, please contact the SFML team")
    #define MML_DEPRECATED

#endif

#endif // MML_CONFIG_HPP
