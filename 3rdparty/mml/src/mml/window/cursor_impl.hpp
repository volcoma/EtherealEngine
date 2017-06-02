#ifndef MML_CURSORIMPL_HPP
#define MML_CURSORIMPL_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/config.hpp>

#if defined(MML_SYSTEM_WINDOWS)
    #include <mml/window/win32/cursor_impl.hpp>
#elif defined(MML_SYSTEM_LINUX) || defined(MML_SYSTEM_FREEBSD)
    #include <mml/window/unix/cursor_impl.hpp>
#endif


#endif // MML_CURSORIMPL_HPP
