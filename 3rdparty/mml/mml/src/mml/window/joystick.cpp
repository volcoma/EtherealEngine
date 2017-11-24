////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/joystick.hpp>
#include <mml/window/joystick_manager.hpp>


namespace mml
{
////////////////////////////////////////////////////////////
bool joystick::is_connected(unsigned int joystick)
{
    return priv::joystick_manager::get_instance().get_state(joystick).connected;
}


////////////////////////////////////////////////////////////
unsigned int joystick::get_button_count(unsigned int joystick)
{
    return priv::joystick_manager::get_instance().get_capabilities(joystick).button_count;
}


////////////////////////////////////////////////////////////
bool joystick::has_axis(unsigned int joystick, axis axis)
{
    return priv::joystick_manager::get_instance().get_capabilities(joystick).axes[axis];
}


////////////////////////////////////////////////////////////
bool joystick::is_button_pressed(unsigned int joystick, unsigned int button)
{
    return priv::joystick_manager::get_instance().get_state(joystick).buttons[button];
}


////////////////////////////////////////////////////////////
float joystick::get_axis_position(unsigned int joystick, axis axis)
{
    return priv::joystick_manager::get_instance().get_state(joystick).axes[axis];
}


////////////////////////////////////////////////////////////
joystick::identification joystick::get_identification(unsigned int joystick)
{
    return priv::joystick_manager::get_instance().get_identification(joystick);
}


////////////////////////////////////////////////////////////
void joystick::update()
{
    return priv::joystick_manager::get_instance().update();
}


////////////////////////////////////////////////////////////
joystick::identification::identification() :
name     ("No joystick"),
vendor_id (0),
product_id(0)
{

}

} // namespace mml
