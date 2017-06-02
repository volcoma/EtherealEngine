#ifndef MML_WINDOWHANDLE_HPP
#define MML_WINDOWHANDLE_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/config.hpp>

// Windows' HWND is a typedef on struct HWND__*
#if defined(MML_SYSTEM_WINDOWS)
    struct HWND__;
#endif

namespace mml
{
#if defined(MML_SYSTEM_WINDOWS)

    // window handle is HWND (HWND__*) on Windows
    typedef HWND__* window_handle;

#elif defined(MML_SYSTEM_LINUX) || defined(MML_SYSTEM_FREEBSD)

    // window handle is window (unsigned long) on Unix - X11
    typedef unsigned long window_handle;

#elif defined(MML_SYSTEM_MACOS)

    // window handle is NSWindow or NSView (void*) on Mac OS X - Cocoa
    typedef void* window_handle;

#endif

} // namespace mml


#endif // MML_WINDOWHANDLE_HPP
