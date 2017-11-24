////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/joystick_manager.hpp>


namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
joystick_manager& joystick_manager::get_instance()
{
    static joystick_manager instance;
    return instance;
}


////////////////////////////////////////////////////////////
const joystick_caps& joystick_manager::get_capabilities(unsigned int joystick) const
{
    return m_joysticks[joystick].capabilities;
}


////////////////////////////////////////////////////////////
const joystick_state& joystick_manager::get_state(unsigned int joystick) const
{
    return m_joysticks[joystick].state;
}


////////////////////////////////////////////////////////////
const joystick::identification& joystick_manager::get_identification(unsigned int joystick) const
{
    return m_joysticks[joystick].identification;
}


////////////////////////////////////////////////////////////
void joystick_manager::update()
{
    for (int i = 0; i < joystick::count; ++i)
    {
        item& item = m_joysticks[i];

        if (item.state.connected)
        {
            // Get the current state of the joystick
            item.state = item.joystick.update();

            // Check if it's still connected
            if (!item.state.connected)
            {
                item.joystick.close();
                item.capabilities   = joystick_caps();
                item.state          = joystick_state();
                item.identification = joystick::identification();
            }
        }
        else
        {
            // Check if the joystick was connected since last update
            if (joystick_impl::is_connected(i))
            {
                if (item.joystick.open(i))
                {
                    item.capabilities   = item.joystick.get_capabilities();
                    item.state          = item.joystick.update();
                    item.identification = item.joystick.get_identification();
                }
            }
        }
    }
}


////////////////////////////////////////////////////////////
joystick_manager::joystick_manager()
{
    joystick_impl::initialize();
}


////////////////////////////////////////////////////////////
joystick_manager::~joystick_manager()
{
    for (int i = 0; i < joystick::count; ++i)
    {
        if (m_joysticks[i].state.connected)
            m_joysticks[i].joystick.close();
    }

    joystick_impl::cleanup();
}

} // namespace priv

} // namespace mml
