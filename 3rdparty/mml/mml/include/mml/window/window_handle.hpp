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
    using window_handle = HWND__ *;

#elif defined(MML_SYSTEM_LINUX) || defined(MML_SYSTEM_FREEBSD) || defined(MML_SYSTEM_OPENBSD)

    // window handle is window (unsigned long) on Unix - X11
    using window_handle =unsigned long;

#elif defined(MML_SYSTEM_MACOS)

    // window handle is NSWindow or NSView (void*) on Mac OS X - Cocoa
    using window_handle = void*;

#endif

} // namespace mml


#endif // MML_WINDOWHANDLE_HPP
