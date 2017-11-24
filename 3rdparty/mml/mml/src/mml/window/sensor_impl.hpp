#ifndef MML_SENSORIMPL_HPP
#define MML_SENSORIMPL_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/config.hpp>
#include <mml/window/sensor.hpp>

#if defined(MML_SYSTEM_WINDOWS)
    #include <mml/window/win32/sensor_impl.hpp>
#elif defined(MML_SYSTEM_LINUX) || defined(MML_SYSTEM_FREEBSD)
    #include <mml/window/unix/sensor_impl.hpp>
#endif


#endif // MML_SENSORIMPL_HPP
