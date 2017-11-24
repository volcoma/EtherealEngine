#ifndef MML_JOYSTICKIMPL_HPP
#define MML_JOYSTICKIMPL_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/config.hpp>
#include <mml/window/joystick.hpp>
#include <algorithm>


namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Structure holding a joystick's information
///
////////////////////////////////////////////////////////////
struct joystick_caps
{
    joystick_caps()
    {
        button_count = 0;
        std::fill(axes, axes + joystick::axis_count, false);
    }

    unsigned int button_count;               ///< Number of buttons supported by the joystick
    bool         axes[joystick::axis_count]; ///< Support for each axis
};


////////////////////////////////////////////////////////////
/// \brief Structure holding a joystick's state
///
////////////////////////////////////////////////////////////
struct joystick_state
{
    joystick_state()
    {
        connected = false;
        std::fill(axes, axes + joystick::axis_count, 0.f);
        std::fill(buttons, buttons + joystick::button_count, false);
    }

    bool  connected;                      ///< Is the joystick currently connected?
    float axes[joystick::axis_count];      ///< Position of each axis, in range [-100, 100]
    bool  buttons[joystick::button_count]; ///< Status of each button (true = pressed)
};

} // namespace priv

} // namespace mml


#if defined(MML_SYSTEM_WINDOWS)
    #include "win32/joystick_impl.hpp"
#elif defined(MML_SYSTEM_LINUX)
	#include "unix/joystick_impl.hpp"
#elif defined(MML_SYSTEM_FREEBSD)
    #include "free_bsd/joystick_impl.hpp"
#endif


#endif // MML_JOYSTICKIMPL_HPP
