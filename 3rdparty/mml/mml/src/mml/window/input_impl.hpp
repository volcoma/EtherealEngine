#ifndef MML_INPUTIMPL_HPP
#define MML_INPUTIMPL_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/config.hpp>

#if defined(MML_SYSTEM_WINDOWS)
    #include <mml/window/win32/input_impl.hpp>
#elif defined(MML_SYSTEM_LINUX) || defined(MML_SYSTEM_FREEBSD)
    #include <mml/window/unix/input_impl.hpp>
#endif


#endif // MML_INPUTIMPL_HPP
