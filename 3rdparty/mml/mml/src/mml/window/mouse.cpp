////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/mouse.hpp>
#include <mml/window/input_impl.hpp>
#include <mml/window/window.hpp>


namespace mml
{
////////////////////////////////////////////////////////////
bool mouse::is_button_pressed(button button)
{
    return priv::input_impl::is_mouse_button_pressed(button);
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> mouse::get_position()
{
    return priv::input_impl::get_mouse_position();
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> mouse::get_position(const window& relativeTo)
{
    return priv::input_impl::get_mouse_position(relativeTo);
}


////////////////////////////////////////////////////////////
void mouse::set_position(const std::array<std::int32_t, 2>& position)
{
    priv::input_impl::set_mouse_position(position);
}


////////////////////////////////////////////////////////////
void mouse::set_position(const std::array<std::int32_t, 2>& position, const window& relativeTo)
{
    priv::input_impl::set_mouse_position(position, relativeTo);
}

} // namespace mml
