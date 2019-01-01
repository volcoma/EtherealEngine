#ifndef MML_CLIPBOARDIMPL_HPP
#define MML_CLIPBOARDIMPL_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/config.hpp>

#if defined(MML_SYSTEM_WINDOWS)
    #include <mml/window/win32/clipboard_impl.hpp>
#elif defined(MML_SYSTEM_LINUX) || defined(MML_SYSTEM_FREEBSD) || defined(MML_SYSTEM_OPENBSD)
    #include <mml/window/unix/clipboard_impl.hpp>
#elif defined(MML_SYSTEM_MACOS)
    #include <mml/window/osx/clipboard_impl.hpp>
#elif defined(MML_SYSTEM_IOS)
    #include <mml/window/ios/clipboard_impl.hpp>
#elif defined(MML_SYSTEM_ANDROID)
    #include <mml/window/android/clipboard_impl.hpp>
#endif

#endif // MML_CLIPBOARDIMPL_HPP
